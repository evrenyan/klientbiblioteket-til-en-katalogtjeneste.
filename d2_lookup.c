/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "d2_lookup.h"

// Funksjon for å rekursivt finne og returnere en node med gitt ID i et tre
Tre *finn_node_i_tre(Tre *rot, uint32_t id)
{
    // Sjekk om gjeldende rot-node har ønsket ID
    if (rot->id == id)
        return rot;
    // Gå gjennom alle barnenodene til den gjeldende rot-noden
    for (uint32_t i = 0; i < rot->antall_children; ++i)
    {
        Tre *resultat = finn_node_i_tre(rot->child[i], id);
        // Hvis node med ønsket ID er funnet, returner resultatet
        if (resultat)
            return resultat;
    }
    return NULL;
}

// Funksjon for å skrive ut treet rekursivt med innrykk basert på nivå
void skrive_ut_tre(Tre *rot, int nv)
{
    if (!rot)
        return;
    for (int i = 0; i < nv; ++i)
        printf("-");
    // Skriv ut verdien til den gjeldende rot-noden
    printf("%u (id: %u)\n", rot->verdi, rot->id);
    // Rekursivt kall for å skrive ut barnenodene med økt nivå
    for (uint32_t i = 0; i < rot->antall_children; ++i)
    {
        skrive_ut_tre(rot->child[i], nv + 1);
    }
}
// Funksjon for å rekursivt slette et tre startet fra den gitte rotnoden
void slett_tre(Tre *rot)
{
    // Gå gjennom alle barnenodene til den gjeldende rot-noden
    for (uint32_t i = 0; i < rot->antall_children; ++i)
    {
        slett_tre(rot->child[i]);
    }
    free(rot);
}
// Hjelpefunksjon for å sette inn en node i treet
void sett_inn_node(LocalTreeStore *tre, NetNode *node)
{

    Tre *gjeldende_node = finn_node_i_tre(tre->root, node->id);
    gjeldende_node->id = node->id;
    gjeldende_node->verdi = node->value;
    gjeldende_node->antall_children = node->num_children;
    for (uint32_t i = 0; i < gjeldende_node->antall_children; ++i)
    {
        // printf("evren\n");
        gjeldende_node->childID[i] = node->child_id[i];
        Tre *barn_node = (Tre *)malloc(sizeof(Tre));
        memset(barn_node, 0, sizeof(Tre));
        barn_node->id = gjeldende_node->childID[i];
        barn_node->antall_children = 0;
        gjeldende_node->child[i] = barn_node;
    }
}

D2Client *d2_client_create(const char *server_name, uint16_t server_port)
{
    D2Client *klient = (D2Client *)malloc(sizeof(D2Client));
    klient->peer = d1_create_client();
    int resltt = d1_get_peer_info(klient->peer, server_name, server_port);
    if (!resltt)
    {
        d1_delete(klient->peer);
        free(klient);
        return NULL;
    }
    return klient;
}

D2Client *d2_client_delete(D2Client *client)
{
    d1_delete(client->peer);
    free(client);
    printf("slettet nu!!\n");
    return NULL;
}

int d2_send_request(D2Client *client, uint32_t id)
{
    PacketRequest foresporsel;
    memset(&foresporsel, 0, sizeof(foresporsel));
    foresporsel.type = htons(TYPE_REQUEST);
    foresporsel.id = htonl(id);
    int resultat = d1_send_data(client->peer, (char *)&foresporsel, sizeof(foresporsel));
    if (resultat <= 0)
        return 0;
    return 1;
}

int d2_recv_response_size(D2Client *client)
{
    PacketResponseSize svar;
    memset(&svar, 0, sizeof(svar));
    int retur_verdi = d1_recv_data(client->peer, (char *)&svar, sizeof(svar));
    if (retur_verdi <= 0)
        return 0;
    svar.type = ntohs(svar.type);
    svar.size = ntohs(svar.size);
    return svar.size;
}

int d2_recv_response(D2Client *client, char *buffer, size_t sz)
{
    return d1_recv_data(client->peer, buffer, sz);
}

LocalTreeStore *d2_alloc_local_tree(int num_nodes)
{
    LocalTreeStore *resultat = (LocalTreeStore *)malloc(sizeof(LocalTreeStore));
    resultat->number_of_nodes = num_nodes;

    Tre *rot_node = (Tre *)malloc(sizeof(Tre));
    memset(rot_node, 0, sizeof(Tre));
    rot_node->id = 0;
    resultat->root = rot_node;
    return resultat;
}

void d2_free_local_tree(LocalTreeStore *nodes)
{

    slett_tre(nodes->root);
    free(nodes);
}

int d2_add_to_local_tree(LocalTreeStore *nodes_out, int node_idx, char *buffer, int buflen)
{

    (void)node_idx;

    NetNode *node;
    int neste_indeks;

    neste_indeks = 0;

    while (neste_indeks < buflen)
    {
        node = (NetNode *)(buffer + neste_indeks);

        // Convert to host order
        node->id = ntohl(node->id);
        neste_indeks += sizeof(node->id);
        node->value = ntohl(node->value);
        neste_indeks += sizeof(node->value);
        node->num_children = ntohl(node->num_children);
        neste_indeks += sizeof(node->num_children);
        for (uint32_t i = 0; i < node->num_children; i++)
        {
            node->child_id[i] = ntohl(node->child_id[i]);
            neste_indeks += sizeof(node->child_id[i]);
        }

        sett_inn_node(nodes_out, node);
    }

    return 0;
}

void d2_print_tree(LocalTreeStore *nodes_out)
{

    skrive_ut_tre(nodes_out->root, 0);
}
