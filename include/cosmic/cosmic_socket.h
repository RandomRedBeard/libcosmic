#ifndef COSMIC_SOCKET_H
#define COSMIC_SOCKET_H

#include <stdlib.h>
#include <string.h>

#include <cosmic/cosmic_io.h>
#include <cosmic/cosmic_lib.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#ifndef SHUT_RD
#define SHUT_RD SD_RECEIVE
#endif

#ifndef SHUT_WR
#define SHUT_WR SD_SEND
#endif

#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif

#define poll WSAPoll
#else
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cosmic_socket cosmic_socket_t;

COSMIC_DLL cosmic_socket_t *cosmic_socket_new(const char *, const char *);
COSMIC_DLL void cosmic_socket_free(cosmic_socket_t *);

/**
 * Connectors
 */

COSMIC_DLL int cosmic_socket_connect(cosmic_socket_t *);
COSMIC_DLL int cosmic_socket_bind(cosmic_socket_t *);
COSMIC_DLL int cosmic_socket_listen(cosmic_socket_t *, int);
COSMIC_DLL cosmic_socket_t *cosmic_socket_accept(cosmic_socket_t *);

/**
 * IO Functions
 */

COSMIC_DLL ssize_t cosmic_socket_read(cosmic_socket_t *, char *, size_t);
COSMIC_DLL ssize_t cosmic_socket_write(cosmic_socket_t *, const char *, size_t);
COSMIC_DLL int cosmic_socket_close(cosmic_socket_t *);

/**
 * Socket specific functions
 */

COSMIC_DLL int cosmic_socket_rpoll(cosmic_socket_t *);
COSMIC_DLL int cosmic_socket_wpoll(cosmic_socket_t *);
COSMIC_DLL int cosmic_socket_shutdown(cosmic_socket_t *, int);

/**
 * Set underlying io features
 */

COSMIC_DLL void cosmic_socket_set_rpwait(cosmic_socket_t *, int);
COSMIC_DLL void cosmic_socket_set_wpwait(cosmic_socket_t *, int);

/* Return underlying io */
COSMIC_DLL cosmic_io_t *cosmic_socket_get_io(cosmic_socket_t *);
/* Return underlying handle */
COSMIC_DLL int cosmic_socket_get_fd(cosmic_socket_t *);

#ifdef __cplusplus
}
#endif

#endif
