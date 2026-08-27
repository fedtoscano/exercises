#define _DEFAULT_SOURCE
#include <arpa/inet.h>  // inet_ntoa, htons, htonl, ntohs
#include <netinet/in.h> // struct sockaddr_in, INADDR_ANY
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // socket, bind, listen, accept, setsockopt, recv, send
#include <sys/types.h>
#include <unistd.h> // close

#define PORT 8080
#define BACKLOG 16   // how many clients can be queued
#define BUFSIZE 4096 // buffer size for incoming data


long TOTAL_CONNECTIONS = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int make_listener(int port) {
  // creating a file descriptor
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  // creates the structure for the socket address
  struct sockaddr_in s;
  memset(&s, 0, sizeof(s)); // sets everything to 0: sanity check
  s.sin_family =
      AF_INET; // socket internet family: Address Family Internet (IPv4)
  /*
   * htonl and htons convert integers to network byte order
   * use this to avoid untracked formatting (reversed numbers ecc)
   * when assigning ports and ips from a machine with different bit management
   * system
   *
   * addresses: Host To Network LONG
   * ports: Host To Newtork SHORT
   * */
  s.sin_addr.s_addr = htonl(INADDR_ANY);
  s.sin_port = htons(port);

  // binds the socket to the socket address structure
  int b = bind(fd, (struct sockaddr *)&s, sizeof(s));
  if (b < 0) {
    perror("bind");
    return -1;
  }

  // make the socket passive: different from a client, this one just listens
  int l = listen(fd, BACKLOG);
  if (l < 0) {
    perror("listen");
    return -1;
  }

  // returns the file descriptor
  return fd;
}

/*
 * This method defines the echo server behaviour: the server
 * just sends back the data to the client
 * */
void send_back_to_client(int cfd, ssize_t n, char *buf) {
  // tracks how many bytes we have sent
  ssize_t sent = 0;

  // the cycle continues until we have sent all the bytes
  while (sent < n) {
    ssize_t s = send(cfd, // the destination is the client file descriptor

                     // in case of non-transmitted bytes, these two
                     // offsets ensure that we repeat the transmission from the
                     // last sent byte
                     buf + sent, 
                     n - sent,

                     // additional flags
                     0
                );

    if (s < 0) {
      perror("send");
      return;
    }

    // updates the total byte sent
    sent += s;
  }
}

void handle_client(int cfd) {
  // creates a buffer with the param BUFSIZE (4096)
  char buf[BUFSIZE];

  // this for-loop goes on indefinitely until the client
  // explicitly closes the connection: this operation
  // makes this server a SEQUENTIAL server (processes one client at a time)
  for (;;) {
    // gets data from client
    ssize_t n = recv(cfd, buf, BUFSIZE, 0);

    // manages return types
    if (n > 0)
      send_back_to_client(cfd, n, buf); // echo server behaviour
    else if (n == 0)
      return; // client has closed the connection
    else if (n < 0) {
      perror("recv");
      return;
    } // error
  }
}

void increment_total_connection(void){
  pthread_mutex_lock(&mtx);
  for(int i = 0; i < 1000; i++) TOTAL_CONNECTIONS++;
  pthread_mutex_unlock(&mtx);
}

void *routine_start(void *arg) { 
  pthread_detach(pthread_self());
  increment_total_connection();
  int cfd = (intptr_t)arg;
  handle_client(cfd);
  close(cfd);
  return NULL; 
}

int main(void) {

  // --- 1. Create LFD (Listen File Descriptor)
  int lfd = make_listener(PORT);
  if (lfd < 0) {
    fprintf(stderr, "impossibile aprire il listener\n");
    return 1;
  }
  printf("in ascolto su :%d (Ctrl-C per fermare)\n", PORT);

  // --- 2. For each connection received, create a CFD (Client File Descriptor)
  for (;;) {
    struct sockaddr_in cli;
    socklen_t len = sizeof cli;

    // Client File Descriptor: specific socket for that particular connection
    // with a client processes the next incoming client
    int cfd = accept(lfd, (struct sockaddr *)&cli, &len);
    if (cfd < 0) {
      perror("accept");
      continue;
    }
    printf("connessione da %s:%d\n", inet_ntoa(cli.sin_addr),
           ntohs(cli.sin_port));

    // create a new empty thread
    pthread_t thread;

    /* 
     * we need to pass the cfd to the thread becuase of its shared state
     * the function pthread_create() accepts a void pointer as argument, so
     * we take an int variable which is as big as a pointer and pass it casted
     * as void pointer (the type accepted)
     *
     * inside routine_start() we get back the int value of the cfd
     * */
    pthread_create(
        &thread,                // ref to the thread variable
        NULL,                   // additioanl flags
        routine_start,          // callback that the thread will execute
        (void *)(intptr_t)cfd   // cfd value casted as void pointer
    );
  }
  return 0;
}
