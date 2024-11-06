/* ======================================================================
 * YOU CAN MODIFY THIS FILE.
 * ====================================================================== */

#ifndef D1_UDP_MOD_H
#define D1_UDP_MOD_H

#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* This structure keeps all information about this client's association
 * with the server in one place.
 * It is expected that d1_create_client() allocates such a D1Peer object
 * dynamically, and that d1_delete() frees it.
 */
struct D1Peer
{
    int32_t            socket;      /* the peer's UDP socket */
    struct sockaddr_in addr;        /* addr of my peer, initialized to zero */
    int                next_seqno;  /* either 0 or 1, initialized to zero */
};

typedef struct D1Peer D1Peer;

#endif /* D1_UDP_MOD_H */

