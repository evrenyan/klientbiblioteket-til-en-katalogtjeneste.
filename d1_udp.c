/* ======================================================================
 * YOU ARE EXPECTED TO MODIFY THIS FILE.
 * ====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "d1_udp.h"
#define _XOPEN_SOURCE 700
#define MAX_PACKET_SIZE 1024

// Hjelpefunksjon for å sjekke dataintegritet
uint16_t checksum(D1Header *h)
{
  uint8_t total_sum[2] = {0};
  uint8_t *pointer = (uint8_t *)h;

  total_sum[0] = pointer[0] ^ pointer[4] ^ pointer[6];
  total_sum[1] = pointer[1] ^ pointer[5] ^ pointer[7];

  uint32_t DataLen = ntohl(h->size) - sizeof(D1Header);
  pointer = pointer + sizeof(D1Header);
  for (uint32_t i = 0; i < DataLen; i += 2)
  {
    total_sum[0] ^= pointer[i];
    if (i + 1 < DataLen)
      total_sum[1] ^= pointer[i + 1];
  }

  return *(uint16_t *)total_sum;
}

D1Peer *d1_create_client()
{

  D1Peer *pr = (D1Peer *)malloc(sizeof(D1Peer));
  pr->socket = 0;
  pr->next_seqno = 0;
  memset(&pr->addr, 0, sizeof(pr->addr));
  return pr;
}

D1Peer *d1_delete(D1Peer *peer)
{
  close(peer->socket);
  free(peer);
  printf("det slettes nu...\n");
  return NULL;
}

int d1_get_peer_info(struct D1Peer *peer, const char *peername, uint16_t server_port)
{
  struct addrinfo hints, *result, *ptr;
  int sockedFd;
  int status;
  char port_str[16];
  sprintf(port_str, "%u", server_port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = 0;

  if ((status = getaddrinfo(peername, port_str, &hints, &result)) != 0)
  {
    fprintf(stderr, "ta adress info feil: : %s\n", gai_strerror(status));
    return 0;
  }

  for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
  {
    printf("evren\n");
    printf("prosess starter...\n");
    if ((sockedFd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1)
    {
      perror("Socket creation error:");
      continue;
    }
    memcpy(&peer->addr, ptr->ai_addr, sizeof(peer->addr));
    break;
  }
  // printf("Det er Her2!!\n");

  if (ptr == NULL)
  {
    fprintf(stderr, "Det gikk feil til å skape sockedFd!!!\n");
    freeaddrinfo(result);
    return 0;
  }

  peer->socket = sockedFd;
  freeaddrinfo(result);
  return 1;
}

int d1_recv_data(struct D1Peer *peer, char *buffer, size_t sz)
{
  size_t maks_storrelse = 1024;
  uint8_t *buf = (uint8_t *)malloc(maks_storrelse);
  if (!buf)
    return -1;

  socklen_t addr_len = sizeof(peer->addr);
  int ret;
  while (1)
  {
    ret = recvfrom(peer->socket, buf, maks_storrelse, 0, (struct sockaddr *)&peer->addr, &addr_len);
    if (ret < (int)sizeof(D1Header))
    {
      free(buf);
      return -1;
    }
    D1Header *h = (D1Header *)buf;
    uint32_t size = ntohl(h->size);
    uint16_t flags = ntohs(h->flags);
    uint16_t sjekkesum = h->checksum;
    if ((int)size != ret)
    {
      free(buf);
      return -2;
    }
    if (sjekkesum == checksum(h))
    {
      d1_send_ack(peer, (flags & SEQNO) ? 1 : 0);
      break;
    }
    // Sender feil sekvensnummer med vilje
    if (flags & SEQNO)
    {
      d1_send_ack(peer, 0);
    }
    else
    {
      d1_send_ack(peer, 1);
    }
  }
  size_t i = ret - sizeof(D1Header);
  if (i < sz)
  {
    // Hvis i er mindre enn sz, beholder vi verdien av i
    i = i;
  }
  else
  {
    // Hvis i er større enn eller lik sz, setter vi i til sz
    i = sz;
  }
  memcpy(buffer, buf + sizeof(D1Header), i);
  free(buf);
  return i;
}

int d1_wait_ack(D1Peer *peer, char *buffer, size_t sz)
{
  (void)buffer;
  (void)sz;

  D1Header *header = (D1Header *)malloc(sizeof(D1Header));

  socklen_t addr_len = sizeof(peer->addr);
  if (recvfrom(peer->socket, header, sizeof(D1Header), 0, (struct sockaddr *)&peer->addr, &addr_len) != sizeof(D1Header))
  {
    free(header);
    return -1;
  }
  uint16_t flags = ntohs(header->flags);
  if (((flags & ACKNO) && (peer->next_seqno)) || (!(flags & ACKNO) && !(peer->next_seqno)))
  {
    peer->next_seqno = 1 - peer->next_seqno;
  }
  free(header);
  return 1;
}

int d1_send_data(D1Peer *peer, char *buffer, size_t sz)
{
  // Beregn størrelsen som skal sendes (datastørrelse + størrelse på D1Header)
  size_t sende_storrelse = sz + sizeof(D1Header);

  // Alloker minne for bufferen som skal sendes
  uint8_t *sende_buffer = (uint8_t *)malloc(sende_storrelse);
  if (!sende_buffer)
    return -1;

  // Kopier data til bufferen med et offset på størrelsen av D1Header
  memcpy(sende_buffer + sizeof(D1Header), buffer, sz);
  D1Header *header = (D1Header *)sende_buffer;
  header->flags = 0;
  header->flags |= FLAG_DATA;
  // Bestem sekvensnummer basert på peer->next_seqno
  int gjeldende_seqno = peer->next_seqno;
  if (gjeldende_seqno == 1)
    header->flags |= SEQNO;
  header->size = (uint32_t)sende_storrelse;

  header->flags = htons(header->flags);
  header->size = htonl(header->size);
  header->checksum = checksum(header);

  int ret;
  while (1)
  {
    ret = (int)sendto(peer->socket, sende_buffer, sende_storrelse, 0, (const struct sockaddr *)&peer->addr, sizeof(peer->addr));
    d1_wait_ack(peer, NULL, 0);
    if (gjeldende_seqno != peer->next_seqno)
      break;
  }
  free(sende_buffer);
  return ret;
}

void d1_send_ack(struct D1Peer *peer, int seqno)
{
  // Opprett ACK-pakke
  D1Header *header = (D1Header *)malloc(sizeof(D1Header));
  // Sett flaggene i headeren for ACK
  header->flags = 0;
  header->flags |= FLAG_ACK;
  if (seqno != 0)
    header->flags |= ACKNO;
  // Setter størrelsen til headeren
  header->size = sizeof(D1Header);

  header->flags = htons(header->flags);
  header->size = htonl(header->size);
  header->checksum = checksum(header);

  sendto(peer->socket, header, sizeof(D1Header), 0, (const struct sockaddr *)&peer->addr, sizeof(peer->addr));
  free(header);
}