#include <stdio.h>
#include <string.h>
#include <enet/enet.h>

#define HOST "localhost"
#define PORT (3000)
#define BUFFERSIZE (1500)
#define PACKET_SIZE (1400)
#define ACK_MSG "acked"

ENetAddress address; // server host and port
ENetHost *server; // server
ENetEvent event;

int main () {
  if (enet_initialize () != 0) {
    printf("an error occurred while initializing ENet.\n");
    return EXIT_FAILURE;
  }

  // server
  address.host = ENET_HOST_ANY;
  enet_address_set_host(&address, HOST);
  address.port = PORT;

  server = enet_host_create(&address, 32, 2, 0, 0);

  if (server == NULL) {
    fprintf(stderr, "an error occurred while trying to create an ENet server host.\n");
    exit(EXIT_FAILURE);
  }

  printf("listening on %x:%u\n", address.host, address.port);

  int counter = 0;
  while (1) {
    while (enet_host_service(server, &event, 5) > 0) {
      switch (event.type) {

        case ENET_EVENT_TYPE_CONNECT:
          printf("A new client connected from %x:%u.\n",
            event.peer -> address.host,
            event.peer -> address.port);
          event.peer -> data = "client";
          break;

        case ENET_EVENT_TYPE_RECEIVE:
          // printf("a packet of length %ld containing %s was received from %s on channel %u.\n",
          //   event.packet -> dataLength,
          //   (char*)event.packet -> data,
          //   (char*)event.peer -> data,
          //   event.channelID);

          if (event.packet -> dataLength != PACKET_SIZE + 1) {
            printf("unexpected packet size, expected: %d, actural: %ld\n", PACKET_SIZE + 1, event.packet -> dataLength);
            return 1;
          }

          counter++;
          printf("counter: %d\n", counter);

          enet_peer_send(server -> peers, 1, enet_packet_create(ACK_MSG, strlen(ACK_MSG) + 1, ENET_PACKET_FLAG_RELIABLE));

          enet_packet_destroy(event.packet);
          break;

        case ENET_EVENT_TYPE_DISCONNECT:
          printf ("%s disconnected.\n", (char*)event.peer -> data);
          /* Reset the peer's client information. */
          event.peer -> data = NULL;
          break;

        default:
          printf("Tick tock.\n");
          break;
      }
    }
  }

  enet_host_destroy(server);
  atexit(enet_deinitialize);
}
