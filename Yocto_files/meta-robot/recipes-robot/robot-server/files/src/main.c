#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "robot_state.h"
#include "lib_audio.h"
#include "lib_leds.h"
#include "auth.h"
#include "api.h"

// Configuraciones del servidor iniciales
#define SERVER_PORT      8080
#define WWW_ROOT         "./www"
#define MAX_UPLOAD_BYTES (32 * 1024)

// Body del POST
typedef struct {
    char  *data;
    size_t used;
} UploadBuf;

// Serving File
static const char *mime_for(const char *path) {
    const char *d = strrchr(path, '.');
    if (!d)                       return "application/octet-stream";
    if (strcmp(d, ".html") == 0)  return "text/html; charset=utf-8";
    if (strcmp(d, ".css")  == 0)  return "text/css";
    if (strcmp(d, ".js")   == 0)  return "application/javascript";
    if (strcmp(d, ".ico")  == 0)  return "image/x-icon";
    if (strcmp(d, ".png")  == 0)  return "image/png";
    if (strcmp(d, ".svg")  == 0)  return "image/svg+xml";
    return "application/octet-stream";
}

static enum MHD_Result serve_file(struct MHD_Connection *conn,
                                   const char *rel)
{
    char path[512];
    snprintf(path, sizeof(path), "%s%s", WWW_ROOT, rel);

    int fd = open(path, O_RDONLY);
    if (fd < 0) return MHD_NO;

    struct stat st;
    if (fstat(fd, &st) < 0 || !S_ISREG(st.st_mode)) { close(fd); return MHD_NO; }

    struct MHD_Response *r =
        MHD_create_response_from_fd((uint64_t)st.st_size, fd);
    MHD_add_response_header(r, "Content-Type",  mime_for(path));
    MHD_add_response_header(r, "Cache-Control", "no-cache");
    enum MHD_Result ret = MHD_queue_response(conn, MHD_HTTP_OK, r);
    MHD_destroy_response(r);
    return ret;
}

static enum MHD_Result send_redirect(struct MHD_Connection *conn,
                                      const char *to)
{
    struct MHD_Response *r =
        MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(r, "Location", to);
    enum MHD_Result ret = MHD_queue_response(conn, MHD_HTTP_FOUND, r);
    MHD_destroy_response(r);
    return ret;
}

static enum MHD_Result send_404(struct MHD_Connection *conn) {
    const char *b = "{\"error\":\"Not found\"}";
    struct MHD_Response *r =
        MHD_create_response_from_buffer(strlen(b), (void *)b,
                                        MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(r, "Content-Type", "application/json");
    enum MHD_Result ret = MHD_queue_response(conn, MHD_HTTP_NOT_FOUND, r);
    MHD_destroy_response(r);
    return ret;
}

static enum MHD_Result send_401(struct MHD_Connection *conn) {
    const char *b = "{\"error\":\"Unauthorized\"}";
    struct MHD_Response *r =
        MHD_create_response_from_buffer(strlen(b), (void *)b,
                                        MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(r, "Content-Type", "application/json");
    enum MHD_Result ret = MHD_queue_response(conn, MHD_HTTP_UNAUTHORIZED, r);
    MHD_destroy_response(r);
    return ret;
}

/* ══════════════════════════════════════════════════════════
   Main request handler / router
══════════════════════════════════════════════════════════ */
static enum MHD_Result handle_request(
    void *cls,
    struct MHD_Connection *conn,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t     *upload_data_size,
    void      **con_cls)
{
    (void)cls; (void)version;

    // First call para los acumuladores
    if (*con_cls == NULL) {
        UploadBuf *buf = calloc(1, sizeof(UploadBuf));
        if (!buf) return MHD_NO;
        *con_cls = buf;
        return MHD_YES;
    }

    UploadBuf *ub = *con_cls;

    /* Acumular los POST body */
    if (*upload_data_size > 0) {
        size_t need = ub->used + *upload_data_size + 1;
        if (need > MAX_UPLOAD_BYTES) return MHD_NO;
        ub->data = realloc(ub->data, need);
        if (!ub->data) return MHD_NO;
        memcpy(ub->data + ub->used, upload_data, *upload_data_size);
        ub->used             += *upload_data_size;
        ub->data[ub->used]   = '\0';
        *upload_data_size    = 0;
        return MHD_YES;
    }

    /* Todos los body recibidos */
    int is_get  = strcmp(method, "GET")  == 0;
    int is_post = strcmp(method, "POST") == 0;
    const char *body = ub->data ? ub->data : "";

    /* Establecer los CORS de seguridad */
    if (strcmp(method, "OPTIONS") == 0) {
        struct MHD_Response *r =
            MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
        MHD_add_response_header(r, "Access-Control-Allow-Origin",  "*");
        MHD_add_response_header(r, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        MHD_add_response_header(r, "Access-Control-Allow-Headers", "Content-Type");
        enum MHD_Result ret = MHD_queue_response(conn, MHD_HTTP_NO_CONTENT, r);
        MHD_destroy_response(r);
        return ret;
    }

    /* ── Rutas publicas ── */
    if (is_get && (strcmp(url, "/") == 0 || strcmp(url, "/index.html") == 0))
        return serve_file(conn, "/login.html");

    if (is_post && strcmp(url, "/api/login") == 0)
        return api_login(conn, body, ub->used);

    /* ── Autenticacion ── */
    if (!auth_check_request(conn)) {
        if (strncmp(url, "/api/", 5) == 0) return send_401(conn);
        return send_redirect(conn, "/");
    }

    /* ── Rutas de paginas protegidas ── */
    if (is_get && strcmp(url, "/dashboard") == 0)
        return serve_file(conn, "/dashboard.html");

    /* ── Rutas de API protegidas ── */
    if (is_post && strcmp(url, "/api/logout") == 0)
        return api_logout(conn);

    if (is_get  && strcmp(url, "/api/status") == 0)
        return api_status(conn);

    if (is_post && strcmp(url, "/api/mode") == 0)
        return api_set_mode(conn, body, ub->used);

    if (is_post && strcmp(url, "/api/move") == 0)
        return api_move(conn, body, ub->used);

    if (is_get  && strcmp(url, "/api/audio/list") == 0)
        return api_audio_list(conn);

    if (is_post && strcmp(url, "/api/audio/control") == 0)
        return api_audio_control(conn, body, ub->used);

    if (is_post && strcmp(url, "/api/audio/volume") == 0)
        return api_audio_volume(conn, body, ub->used);

    return send_404(conn);
}

static void request_done(void *cls, struct MHD_Connection *conn,
                          void **con_cls,
                          enum MHD_RequestTerminationCode toe)
{
    (void)cls; (void)conn; (void)toe;
    UploadBuf *ub = *con_cls;
    if (ub) { free(ub->data); free(ub); *con_cls = NULL; }
}

/* ══════════════════════════════════════════════════════════
   Hilo de actualizacion
══════════════════════════════════════════════════════════ */
static volatile int g_running = 1;

static void *uptime_thread(void *arg) {
    (void)arg;
    while (g_running) {
        sleep(1);
        RobotState *rs = robot_state_get();
        if (rs) {
            pthread_mutex_lock(&rs->lock);
            rs->uptime_secs++;
            pthread_mutex_unlock(&rs->lock);
        }
    }
    return NULL;
}

/* ══════════════════════════════════════════════════════════
   Hilo watchdog, para volver al modo autonomo una vez
   no hay usuarios logueados en el servidor
══════════════════════════════════════════════════════════ */
#define WATCHDOG_INTERVAL_SECS 7

static void set_auto_mode(const char *reason) {
    RobotState *rs = robot_state_get();
    if (!rs) return;
    pthread_mutex_lock(&rs->lock);
    if (rs->mode != MODE_AUTONOMOUS) {
        rs->mode            = MODE_AUTONOMOUS;
        rs->leds.autonomous = 1;
        rs->leds.manual     = 0;
        pthread_mutex_unlock(&rs->lock);     
        printf("[watchdog] %s → modo AUTONOMO\n", reason);
        lib_audio_notify(NOTIFY_AUTONOMOUS);  
        return;
    }
    pthread_mutex_unlock(&rs->lock);
}

static void *watchdog_thread(void *arg) {
    (void)arg;
    while (g_running) {
        sleep(WATCHDOG_INTERVAL_SECS);
        int sesiones = auth_active_sessions();
        // printf("[watchdog] tick — sesiones activas: %d\n", sesiones);
        if (sesiones == 0)
            set_auto_mode("sin sesiones activas");
    }
    return NULL;
}

// Handler de la signal
static struct MHD_Daemon *g_daemon = NULL;
static void on_signal(int s) {
    (void)s;
    g_running = 0;
    if (g_daemon) MHD_stop_daemon(g_daemon);
    printf("\n[server] desconectando servidor\n");
}

/* ══════════════════════════════════════════════════════════
   Servidor MAIN
══════════════════════════════════════════════════════════ */
int main(void) {
    printf("╔════════════════════════════════════╗\n");
    printf("║  Servidor Vaccum Robot             ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    if (robot_state_init() < 0)  { fprintf(stderr, "[main] estado inicial fallo\n");}
    if (lib_audio_init(NULL) < 0)  { fprintf(stderr, "[main] audio init failed\n");}
    if (auth_init()        < 0)  { fprintf(stderr, "[main] autorizacion inicial fallo\n");}
    if (lib_leds_init() < 0) { /* solo warning, no falla fatal */ } 

    // Notificacion de encendido
    lib_audio_notify(NOTIFY_STARTUP); 

    // Iniciar en modo autonomo
    {
        RobotState *rs = robot_state_get();
        if (rs) {
            pthread_mutex_lock(&rs->lock);
            rs->mode = MODE_AUTONOMOUS;
            pthread_mutex_unlock(&rs->lock);
        }
        printf("[main] Modo inicial: AUTONOMO\n");
        lib_audio_notify(NOTIFY_AUTONOMOUS);
    }

    lib_leds_sync_from_state();
    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);

    // Inicializar Threads
    pthread_t tid_uptime, tid_watchdog;
    pthread_create(&tid_uptime,   NULL, uptime_thread,   NULL);
    pthread_create(&tid_watchdog, NULL, watchdog_thread, NULL);

    g_daemon = MHD_start_daemon(
        MHD_USE_THREAD_PER_CONNECTION | MHD_USE_INTERNAL_POLLING_THREAD,
        SERVER_PORT,
        NULL, NULL,
        handle_request, NULL,
        MHD_OPTION_NOTIFY_COMPLETED, request_done, NULL,
        MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int)30,
        MHD_OPTION_END
    );

    if (!g_daemon) {
        fprintf(stderr, "[main] fallo al iniciar el daemon en puerto %d\n", SERVER_PORT);
        auth_destroy();
        lib_audio_destroy();
        robot_state_destroy();
        return 1;
    }

    printf("[server] Servidor escuchando en puerto %d\n\n", SERVER_PORT);
    printf("  Dev:               http://localhost:%d\n", SERVER_PORT);
    printf("  Red local:         http://<hostname -I>:%d\n",   SERVER_PORT);

    while (g_running) sleep(1);

    g_running = 0;
    pthread_join(tid_uptime,   NULL);
    pthread_join(tid_watchdog, NULL);
    auth_destroy();
    lib_audio_destroy();
    robot_state_destroy();
    lib_leds_destroy();
    printf("[server] Close.\n");
    return 0;
}
