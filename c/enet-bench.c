#include <stdio.h>
#include <string.h>
#include <enet/enet.h>

#define HOST "localhost"
#define PORT (3000)
#define BUFFERSIZE (1500)

ENetAddress address; // server host and port
ENetHost *client; // client
ENetPeer *peer; // conntected peer
ENetEvent event;

char* string_repeat(int n, const char* src) {
  size_t slen = strlen(src);
  char* dest = malloc(n*slen);

  int i; char * p;
  for( i=0, p = dest; i < n; i++, p += slen ) {
    memcpy(p, src, slen);
  }
  *p = '\0';
  return dest;
}

int main () {
  if (enet_initialize () != 0) {
    printf("an error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }

  // client
  client = enet_host_create(NULL, 1, 2, 0, 0);
  if (client == NULL) {
    fprintf(stderr, "an error occurred while trying to create an ENet client host.\n");
    exit(EXIT_FAILURE);
  }

  enet_address_set_host(&address, HOST);
  address.port = PORT;

  peer = enet_host_connect(client, &address, 2, 0);

  if (peer == NULL) {
    fprintf (stderr, "no available peers for initiating an ENet connection.\n");
    exit(EXIT_FAILURE);
  }

  // packet
  const unsigned char* data = string_repeat(5, "x");
  ENetPacket *packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);

  if (enet_host_service(client, &event, 5000) > 0  && event.type == ENET_EVENT_TYPE_CONNECT) {
    printf("Connection to %s succeeded.\n", HOST);
    enet_peer_send(peer, 0, packet);
  } else {
    enet_peer_reset(peer);
    printf("Could not connect to %s.\n", HOST);
    exit(EXIT_FAILURE);
  }

  while (1) {
    while (enet_host_service(client, &event, 1000) > 0) {
      switch (event.type)
      {
        case ENET_EVENT_TYPE_NONE:
          break;
        case ENET_EVENT_TYPE_CONNECT:
          break;
        case ENET_EVENT_TYPE_RECEIVE:
          puts((char*) event.packet->data);
          break;
        case ENET_EVENT_TYPE_DISCONNECT:
          printf("You have been disconnected.\n");
          return 0;
      }
    }
  }

  enet_host_destroy(client);
  atexit (enet_deinitialize);
}
