/* ======================================================================
 * YOU CAN MODIFY THIS FILE.
 * ====================================================================== */

#ifndef D2_LOOKUP_MOD_H
#define D2_LOOKUP_MOD_H

#include "d1_udp.h"

struct D2Client
{
    D1Peer *peer;
};
typedef struct D2Client D2Client;

struct Tre
{
    uint32_t id;
    uint32_t verdi;
    uint32_t antall_children;
    struct Tre *child[5];
    uint32_t childID[5];
};

typedef struct Tre Tre;

struct LocalTreeStore
{
    Tre *root;
    int number_of_nodes;
};

typedef struct LocalTreeStore LocalTreeStore;

#endif /* D2_LOOKUP_MOD_H */
