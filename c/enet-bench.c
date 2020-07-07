#include <stdio.h>
#include <string.h>
#include <time.h>
#include <enet/enet.h>

#define HOST "localhost"
#define PORT (3000)
#define BUFFERSIZE (1500)

ENetAddress address; // server host and port
ENetHost *client; // client
ENetPeer *peer; // conntected peer
ENetEvent event;

const int expected_packet_size = 1400;
unsigned char *msg;
ENetPacket *packet;

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

int bench(const unsigned char *name, double benchtime) {
  clock_t start_t = clock();

  // msg = string_repeat(expected_packet_size, "x");
  // packet = enet_packet_create(msg, strlen(msg) + 1, ENET_PACKET_FLAG_RELIABLE);

  // 1400 letters message
  const unsigned char message[] = "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello";
  packet = enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE);

  int loops = 0;
  while (clock() < start_t + benchtime) {

    enet_peer_send(peer, 0, packet);

    while(enet_host_service(client, &event, 5) > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_NONE:
          break;

        case ENET_EVENT_TYPE_CONNECT:
          break;

        case ENET_EVENT_TYPE_RECEIVE:
          // printf("a packet of length %ld containing %s was received from %s on channel %u.\n",
          //   event.packet -> dataLength,
          //   (char*)event.packet -> data,
          //   (char*)event.peer -> data,
          //   event.channelID);
          if (event.packet -> dataLength != expected_packet_size + 1) {
            printf("unexpected packet size, expected: %d, actural: %ld\n", expected_packet_size, event.packet -> dataLength);
            return 1;
          }
          loops += 1;
          break;

        case ENET_EVENT_TYPE_DISCONNECT:
          printf("You have been disconnected.\n");
          return 0;
      }
    }
    // enet_peer_send(peer, 0, packet);
    // enet_host_flush(client);
    // loops++;
  }

  int loop_per_s = loops * CLOCKS_PER_SEC / benchtime;
  int time_per_op = benchtime / loops;
  printf("%s:   %d   %d loops/s   %d ms/op\n", name, loops, loop_per_s, time_per_op);
  return 0;
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

  peer = enet_host_connect(client, &address, 3, 0);

  if (peer == NULL) {
    fprintf (stderr, "no available peers for initiating an ENet connection.\n");
    exit(EXIT_FAILURE);
  }

  if (enet_host_service(client, &event, 5000) > 0  && event.type == ENET_EVENT_TYPE_CONNECT) {
    printf("Connection to %s succeeded.\n", HOST);
  } else {
    enet_peer_reset(peer);
    printf("Could not connect to %s.\n", HOST);
    exit(EXIT_FAILURE);
  }

  double benchtime = 10 * CLOCKS_PER_SEC; // 10s
  if (bench("BenchmarkWriteReliablePacket-ENet-C", benchtime) > 0) {
    return 1;
  }

  enet_host_destroy(client);
  atexit(enet_deinitialize);
}
