#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

// Tabla de Usuarios Registrados
typedef struct { const char *user; const char *pass; } UserEntry;
static const UserEntry USERS[] = {
    { "user1", "password1" },
};

// Calcula cuantos usuarios hay en la tabla
static const int NUM_USERS = (int)(sizeof(USERS) / sizeof(USERS[0]));

// Sistema de sesiones
typedef struct {
    char   token[SESSION_TOKEN_LEN + 1]; //identificador
    char   username[64];
    time_t last_seen;
    int    active;
} Session;

// Array para las sesiones activas
static Session         g_sessions[MAX_SESSIONS];
// Mutex para cuando varios usuarios ingresen a la vez
static pthread_mutex_t g_lock;

// Helpers

// Generar el token de la sesion
static void gen_token(char *buf) {
    const char hex[] = "0123456789abcdef";
    for (int i = 0; i < SESSION_TOKEN_LEN; i++)
        buf[i] = hex[rand() % 16];
    buf[SESSION_TOKEN_LEN] = '\0';
}

// Buscar espacio libre para una sesion
static Session *find_free_slot(void) {
    // Buscar slot vacio
    for (int i = 0; i < MAX_SESSIONS; i++)
        if (!g_sessions[i].active) return &g_sessions[i];
    // En caso de que todos los slots esten ocupados
    Session *oldest = &g_sessions[0];
    for (int i = 1; i < MAX_SESSIONS; i++)
        if (g_sessions[i].last_seen < oldest->last_seen)
            oldest = &g_sessions[i];
    return oldest;
}

// Buscar la sesion por token
static Session *find_by_token(const char *token) {
    if (!token) return NULL;
    for (int i = 0; i < MAX_SESSIONS; i++)
        if (g_sessions[i].active &&
            strncmp(g_sessions[i].token, token, SESSION_TOKEN_LEN) == 0)
            return &g_sessions[i];
    return NULL;
}

// Expirar sesiones viejas
static void expire_old(void) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_SESSIONS; i++)
        if (g_sessions[i].active &&
            (now - g_sessions[i].last_seen) > SESSION_TTL_SECS)
            g_sessions[i].active = 0;
}

/////////////////// API Publica ////////////////////

int auth_init(void) {
    srand((unsigned)time(NULL));
    memset(g_sessions, 0, sizeof(g_sessions));
    if (pthread_mutex_init(&g_lock, NULL) != 0) return -1;
    printf("[auth] inicializado (%d usuario(s))\n", NUM_USERS);
    return 0;
}

// Destruir Servidor
void auth_destroy(void) {
    pthread_mutex_destroy(&g_lock);
}


// Validar el LOGIN 
int auth_validate(const char *username, const char *password) {
    if (!username || !password) return 0;
    for (int i = 0; i < NUM_USERS; i++)
        if (strcmp(USERS[i].user, username) == 0 &&
            strcmp(USERS[i].pass, password) == 0)
            return 1;
    return 0;
}

// Crear sesion para el usuario 
const char *auth_create_session(const char *username) {
    static char buf[SESSION_TOKEN_LEN + 1];
    pthread_mutex_lock(&g_lock);
    expire_old();
    Session *s = find_free_slot();
    gen_token(s->token);
    strncpy(s->username, username, sizeof(s->username) - 1);
    s->username[sizeof(s->username) - 1] = '\0';
    s->last_seen = time(NULL);
    s->active    = 1;
    strncpy(buf, s->token, sizeof(buf));
    pthread_mutex_unlock(&g_lock);
    printf("[auth] session created for '%s'\n", username);
    return buf;
}

// Verificar sesion en una request
int auth_check_request(struct MHD_Connection *conn) {
    const char *cookie = MHD_lookup_connection_value(
        conn, MHD_COOKIE_KIND, "session");
    if (!cookie) return 0;
    pthread_mutex_lock(&g_lock);
    expire_old();
    Session *s = find_by_token(cookie);
    if (s) s->last_seen = time(NULL);
    pthread_mutex_unlock(&g_lock);
    return s != NULL;
}

// Cerrar o Invalidar la sesion 
void auth_invalidate(const char *token) {
    pthread_mutex_lock(&g_lock);
    Session *s = find_by_token(token);
    if (s) { s->active = 0; printf("[auth] session invalidated\n"); }
    pthread_mutex_unlock(&g_lock);
}