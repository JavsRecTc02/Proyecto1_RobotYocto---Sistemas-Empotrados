#ifndef AUTH_H
#define AUTH_H

#include <microhttpd.h>

#define SESSION_TOKEN_LEN  32
#define SESSION_TTL_SECS   3600
#define MAX_SESSIONS       16

int         auth_init(void);
void        auth_destroy(void);
int         auth_validate(const char *username, const char *password);
const char *auth_create_session(const char *username);
int         auth_check_request(struct MHD_Connection *connection);
void        auth_invalidate(const char *token);

#endif