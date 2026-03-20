#include "api.h"
#include "auth.h"
#include "robot_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* ═══════════════════════════════════════════════════════════
   Definicion de helpers internos
═══════════════════════════════════════════════════════════ */

static enum MHD_Result send_json(struct MHD_Connection *conn,
                                  unsigned int status,
                                  const char *body)
{
    struct MHD_Response *r =
        MHD_create_response_from_buffer(strlen(body), (void *)body,
                                        MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header(r, "Content-Type",  "application/json");
    MHD_add_response_header(r, "Cache-Control", "no-cache");
    enum MHD_Result ret = MHD_queue_response(conn, status, r);
    MHD_destroy_response(r);
    return ret;
}

/* ── Extractores de campos JSON mínimos ── */

// Strings
static int json_str(const char *json, const char *key,
                    char *out, size_t out_len)
{
    char needle[128];
    snprintf(needle, sizeof(needle), "\"%s\"", key);
    const char *p = strstr(json, needle);
    if (!p) return 0;
    p += strlen(needle);
    while (*p == ' ' || *p == ':' || *p == ' ') p++;
    if (*p != '"') return 0;
    p++;
    size_t i = 0;
    while (*p && *p != '"' && i < out_len - 1) out[i++] = *p++;
    out[i] = '\0';
    return 1;
}

// Enteros
static int json_int(const char *json, const char *key, int *out)
{
    char needle[128], buf[32];
    snprintf(needle, sizeof(needle), "\"%s\"", key);
    const char *p = strstr(json, needle);
    if (!p) return 0;
    p += strlen(needle);
    while (*p == ' ' || *p == ':') p++;
    if (*p == '"') p++;
    size_t i = 0;
    while (*p && ((*p >= '0' && *p <= '9') || *p == '-') && i < sizeof(buf)-1)
        buf[i++] = *p++;
    buf[i] = '\0';
    if (i == 0) return 0;
    *out = atoi(buf);
    return 1;
}

/* ═══════════════════════════════════════════════════════════
   POST /api/login
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_login(struct MHD_Connection *conn,
                           const char *body, size_t len)
{
    (void)len;
    if (!body || !*body)
        return send_json(conn, MHD_HTTP_BAD_REQUEST,
                         "{\"error\":\"Empty body\"}");

    char username[64] = {0}, password[128] = {0};
    json_str(body, "username", username, sizeof(username));
    json_str(body, "password", password, sizeof(password));

    /* ── Protección fuerza bruta: obtener IP del cliente ── */
    const union MHD_ConnectionInfo *ci =
        MHD_get_connection_info(conn, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
    char client_ip[46] = "unknown";
    if (ci && ci->client_addr) {
        struct sockaddr *sa = (struct sockaddr *)ci->client_addr;
        if (sa->sa_family == AF_INET) {
            struct sockaddr_in *s4 = (struct sockaddr_in *)sa;
            unsigned char *b = (unsigned char *)&s4->sin_addr;
            snprintf(client_ip, sizeof(client_ip),
                     "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
        }
    }

    if (!auth_ip_allowed(client_ip))
        return send_json(conn, 429,
                         "{\"error\":\"Demasiados intentos. Intente mas tarde.\"}");

    if (!auth_validate(username, password)) {
        printf("[api] login FALLIDO para '%s' desde %s\n", username, client_ip);
        auth_ip_failed(client_ip);
        return send_json(conn, MHD_HTTP_UNAUTHORIZED,
                         "{\"error\":\"Credenciales invalidas\"}");
    }

    /* ── Crear sesión ── */
    char token[SESSION_TOKEN_LEN + 1];
    AuthResult res = auth_create_session(username, token);

    if (res == AUTH_ERR_FULL)
        return send_json(conn, 503,
                         "{\"error\":\"Servidor lleno. Maximo de usuarios activos alcanzado.\"}");

    auth_ip_success(client_ip);

    char cookie[160];
    snprintf(cookie, sizeof(cookie),
             "session=%s; HttpOnly; Path=/; Max-Age=%d",
             token, SESSION_TTL_SECS);

    char resp[128];
    snprintf(resp, sizeof(resp),
             "{\"ok\":true,\"username\":\"%s\"}", username);

    struct MHD_Response *r =
        MHD_create_response_from_buffer(strlen(resp), (void *)resp,
                                        MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header(r, "Content-Type",  "application/json");
    MHD_add_response_header(r, "Set-Cookie",    cookie);
    MHD_add_response_header(r, "Cache-Control", "no-cache");
    enum MHD_Result ret = MHD_queue_response(conn, MHD_HTTP_OK, r);
    MHD_destroy_response(r);
    return ret;
}

/* ═══════════════════════════════════════════════════════════
   POST /api/logout
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_logout(struct MHD_Connection *conn)
{
    const char *cookie =
        MHD_lookup_connection_value(conn, MHD_COOKIE_KIND, "session");
    if (cookie) auth_invalidate(cookie);

    struct MHD_Response *r =
        MHD_create_response_from_buffer(11, (void *)"{\"ok\":true}",
                                        MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(r, "Content-Type", "application/json");
    MHD_add_response_header(r, "Set-Cookie",
                            "session=; Max-Age=0; Path=/");
    enum MHD_Result ret = MHD_queue_response(conn, MHD_HTTP_OK, r);
    MHD_destroy_response(r);
    return ret;
}

/* ═══════════════════════════════════════════════════════════
   Metodo GET para /api/status
   Retorna el estado actual del robot en formato JSON
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_status(struct MHD_Connection *conn)
{
    RobotState *rs = robot_state_get();
    if (!rs) return send_json(conn, 500, "{\"error\":\"Estado Invalido\"}");

    pthread_mutex_lock(&rs->lock);

    char buf[1024];
    snprintf(buf, sizeof(buf),
        "{"
          "\"modo\":\"%s\","
          "\"uptime\":%llu,"
          "\"sensores\":{"
            "\"frontal\":%.1f,"
            "\"trasero\":%.1f,"
            "\"izquierda\":%.1f,"
            "\"derecha\":%.1f"
          "},"
          "\"leds\":{"
            "\"power\":%s,"
            "\"autonomo\":%s,"
            "\"manual\":%s,"
            "\"obstaculo\":%s"
          "},"
          "\"audio\":{"
            "\"estado\":%d,"
            "\"cancion_actual_id\":%d,"
            "\"posicion\":%.1f,"
            "\"volumen\":%d"
          "},"
          "\"map\":{"
            "\"robot_x\":%d,"
            "\"robot_y\":%d,"
            "\"robot_head\":%d"
          "}"
        "}",
        rs->mode == MODE_AUTONOMOUS ? "autonomo" : "manual",
        (unsigned long long)rs->uptime_secs,
        rs->sensors.front_cm, rs->sensors.back_cm,
        rs->sensors.left_cm,  rs->sensors.right_cm,
        rs->leds.power     ? "true" : "false",
        rs->leds.autonomous? "true" : "false",
        rs->leds.manual    ? "true" : "false",
        rs->leds.obstacle  ? "true" : "false",
        rs->audio.status, rs->audio.current_track_id,
        rs->audio.position_secs, rs->audio.volume,
        rs->map.robot_x, rs->map.robot_y, rs->map.robot_heading
    );

    pthread_mutex_unlock(&rs->lock);
    return send_json(conn, MHD_HTTP_OK, buf);
}

/* ═══════════════════════════════════════════════════════════
   Metodo POST para seleccion de modo en /api/mode 
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_set_mode(struct MHD_Connection *conn,
                              const char *body, size_t len)
{
    (void)len;
    char mode_str[32] = {0};
    json_str(body, "mode", mode_str, sizeof(mode_str));

    RobotState *rs = robot_state_get();
    if (!rs) return send_json(conn, 500, "{\"error\":\"Estado Invalido\"}");

    pthread_mutex_lock(&rs->lock);

    if (strcmp(mode_str, "manual") == 0) {
        rs->mode            = MODE_MANUAL;
        rs->leds.autonomous = 0;
        rs->leds.manual     = 1;
        printf("[api] mode -> MANUAL\n");
        /*
         * TODO: notify autonomous FSM to stop
         *   lib_set_mode(MODE_MANUAL);
         */
    } else {
        rs->mode            = MODE_AUTONOMOUS;
        rs->leds.autonomous = 1;
        rs->leds.manual     = 0;
        printf("[api] mode -> AUTONOMO\n");
        /*
         * TODO: start autonomous FSM
         *   lib_set_mode(MODE_AUTONOMOUS);
         */
    }

    pthread_mutex_unlock(&rs->lock);
    return send_json(conn, MHD_HTTP_OK, "{\"ok\":true}");
}

/* ═══════════════════════════════════════════════════════════
   Metodo POST para la posicion /api/move 
   Tambien cuenta con velocidad 0-100
   Solo se activa con el modo manual
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_move(struct MHD_Connection *conn,
                          const char *body, size_t len)
{
    (void)len;
    RobotState *rs = robot_state_get();
    if (!rs) return send_json(conn, 500, "{\"error\":\"Estado Invalido\"}");

    pthread_mutex_lock(&rs->lock);
    OperationMode current_mode = rs->mode;
    pthread_mutex_unlock(&rs->lock);

    if (current_mode != MODE_MANUAL)
        return send_json(conn, MHD_HTTP_FORBIDDEN,
                         "{\"error\":\"No se encuentra en modo manual\"}");

    char direction[32] = {0};
    int  speed         = 50;
    json_str(body, "direccion", direction, sizeof(direction));
    json_int(body, "velocidad",    &speed);

    printf("[api] mover -> direccion='%s'  velocidad=%d%%\n", direction, speed);

    // Llamar funcionalidad con la Biblioteca

    return send_json(conn, MHD_HTTP_OK, "{\"ok\":true}");
}

/* ═══════════════════════════════════════════════════════════
   Metodo GET para /api/audio/list
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_audio_list(struct MHD_Connection *conn)
{
    RobotState *rs = robot_state_get();
    if (!rs) return send_json(conn, 500, "{\"error\":\"Estado Invalido\"}");

    pthread_mutex_lock(&rs->lock);

    // Llamar Funcionalidad de musica con la Biblioteca

    if (rs->audio.track_count == 0) {
        pthread_mutex_unlock(&rs->lock);
        return send_json(conn, MHD_HTTP_OK, "{\"tracks\":[]}");
    }

    size_t bufsz = (size_t)rs->audio.track_count * 160 + 32;
    char  *buf   = malloc(bufsz);
    if (!buf) { pthread_mutex_unlock(&rs->lock); return MHD_NO; }

    int n = snprintf(buf, bufsz, "{\"tracks\":[");
    for (int i = 0; i < rs->audio.track_count; i++) {
        AudioTrack *t = &rs->audio.tracks[i];
        n += snprintf(buf + n, bufsz - (size_t)n,
            "%s{\"id\":%d,\"nombre\":\"%s\",\"duracion\":\"%d:%02d\"}",
            i > 0 ? "," : "",
            t->id, t->filename,
            t->duration_secs / 60, t->duration_secs % 60);
    }
    snprintf(buf + n, bufsz - (size_t)n, "]}");

    pthread_mutex_unlock(&rs->lock);

    enum MHD_Result ret = send_json(conn, MHD_HTTP_OK, buf);
    free(buf);
    return ret;
}

/* ═══════════════════════════════════════════════════════════
   Metodo POST para /api/audio/control
   Controlar musica, pausa, play, siguiente, etc
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_audio_control(struct MHD_Connection *conn,
                                   const char *body, size_t len)
{
    (void)len;
    char action[32] = {0};
    int  track_id   = -1;
    json_str(body, "action",   action,   sizeof(action));
    json_int(body, "track_id", &track_id);

    RobotState *rs = robot_state_get();
    if (!rs) return send_json(conn, 500, "{\"error\":\"Estado Invalido\"}");

    pthread_mutex_lock(&rs->lock);

    if (strcmp(action, "play") == 0) {
        rs->audio.current_track_id = track_id;
        rs->audio.status           = AUDIO_PLAYING;
        rs->audio.position_secs    = 0.0f;
        printf("[api] audio -> PLAY  track_id=%d\n", track_id);

        // Funcionalidad de play en la biblioteca

    } else if (strcmp(action, "pause") == 0) {
        rs->audio.status = AUDIO_PAUSED;
        printf("[api] audio -> PAUSE\n");
        
        // Funcionalidad de pause en la biblioteca

    } else if (strcmp(action, "descripcion") == 0) {
        rs->audio.status = AUDIO_PLAYING;
        printf("[api] audio -> descripcion\n");

        // Funcion de descripcion en la biblioteca

    } else if (strcmp(action, "stop") == 0) {
        rs->audio.status           = AUDIO_STOPPED;
        rs->audio.current_track_id = -1;
        rs->audio.position_secs    = 0.0f;
        printf("[api] audio -> STOP\n");

        // Funcion de stop en la biblioteca

    } else {
        pthread_mutex_unlock(&rs->lock);
        return send_json(conn, MHD_HTTP_BAD_REQUEST,
                         "{\"error\":\"Evento desconocido\"}");
    }

    pthread_mutex_unlock(&rs->lock);
    return send_json(conn, MHD_HTTP_OK, "{\"ok\":true}");
}

/* ═══════════════════════════════════════════════════════════
    Metodo POST para /api/audio/volume
    Control de volumen 0-100
═══════════════════════════════════════════════════════════ */
enum MHD_Result api_audio_volume(struct MHD_Connection *conn,
                                  const char *body, size_t len)
{
    (void)len;
    int vol = 70;
    json_int(body, "volume", &vol);
    if (vol < 0)   vol = 0;
    if (vol > 100) vol = 100;

    RobotState *rs = robot_state_get();
    if (!rs) return send_json(conn, 500, "{\"error\":\"Estado Invalido\"}");

    pthread_mutex_lock(&rs->lock);
    rs->audio.volume = vol;
    pthread_mutex_unlock(&rs->lock);

    printf("[api] audio -> VOLUMEN %d%%\n", vol);

    // Funcionalidad del volumen en la biblioteca

    return send_json(conn, MHD_HTTP_OK, "{\"ok\":true}");
}