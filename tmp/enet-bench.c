#include <stdio.h>
#include <string.h>
#include <time.h>
#include <enet/enet.h>

#define HOST "localhost"
#define PORT (3000)
#define BUFFERSIZE (1500)
#define PACKET_SIZE (1400)
#define ACK_MSG "acked"

ENetAddress address; // server host and port
ENetHost *client; // client
ENetPeer *peer; // conntected peer
ENetEvent event;

int serviceResult;

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

  // msg = string_repeat(PACKET_SIZE, "x");
  // packet = enet_packet_create(msg, strlen(msg) + 1, ENET_PACKET_FLAG_RELIABLE);

  // 1400 letters message
  const unsigned char message[] = "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello";

  int loops = 0;
  while (clock() < start_t + benchtime) {

    serviceResult = 1;
    enet_peer_send(peer, 0, enet_packet_create(message, strlen(message) + 1, ENET_PACKET_FLAG_RELIABLE));

    while(serviceResult > 0) {
      serviceResult = enet_host_service(client, &event, 5);

      if (serviceResult > 0) {
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

            if (event.packet -> dataLength != strlen(ACK_MSG) + 1) {
              printf("failed to ack, msg: %s \n", (char*)event.packet -> data);
              return 1;
            }

            loops += 1;
            printf("loops: %d\n", loops);

            enet_packet_destroy(event.packet);
            break;

          case ENET_EVENT_TYPE_DISCONNECT:
            printf("You have been disconnected.\n");
            return 1;

          default:
            printf("default case.\n");
            return 1;
        }
      } else if (serviceResult < 0){
        printf("service failed, result: %d\n", serviceResult);
        return 1;
      }
    }
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
    event.peer -> data = "server";
    printf("Connection to %s succeeded.\n", HOST);
  } else {
    enet_peer_reset(peer);
    printf("Could not connect to %s.\n", HOST);
    exit(EXIT_FAILURE);
  }

  // double benchtime = 10 * CLOCKS_PER_SEC; // 10s
  double benchtime = CLOCKS_PER_SEC; // 10s
  if (bench("BenchmarkWriteReliablePacket-ENet-C", benchtime) > 0) {
    return 1;
  }

  enet_host_destroy(client);
  atexit(enet_deinitialize);
}
