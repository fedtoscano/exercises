#define _DEFAULT_SOURCE
#include <arpa/inet.h>  // inet_ntoa, htons, htonl, ntohs
#include <netinet/in.h> // struct sockaddr_in, INADDR_ANY
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h> // socket, bind, listen, accept, setsockopt, recv, send
#include <sys/types.h>
#include <unistd.h> // close

#define PORT 8080
#define BACKLOG 16   // how many clients can be queued
#define BUFSIZE 4096 // buffer size for incoming data
#define MAX_FDS 256  // max file descriptors allowed

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
                     buf + sent, n - sent,

                     // additional flags
                     0);

    if (s < 0) {
      perror("send");
      return;
    }

    // updates the total byte sent
    sent += s;
  }
}

int main(void) {

  // --- 1. Create LFD (Listen File Descriptor)
  int lfd = make_listener(PORT);
  if (lfd < 0) {
    fprintf(stderr, "impossibile aprire il listener\n");
    return 1;
  }
  printf("in ascolto su :%d (Ctrl-C per fermare)\n", PORT);

  struct pollfd fds[MAX_FDS];
  fds[0].fd = lfd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  // number of file descriptors
  int nfds = 1;
  // --- 2. For each connection received, create a CFD (Client File Descriptor)
  for (;;) {
    poll(fds, nfds, -1);

    if ((fds[0].revents & POLLIN) != 0) {
      // create new client
      struct sockaddr_in cli;
      socklen_t len = sizeof cli;
      int new_cfd = accept(fds[0].fd, (struct sockaddr *)&cli, &len);

      fds[nfds].fd = new_cfd;
      fds[nfds].events = POLLIN;
      fds[nfds].revents = 0;
      nfds++;
    }

    for (int i = 1; i <= nfds - 1; i++) {
      if (fds[i].revents & POLLIN) {
        char buf[BUFSIZE];
        ssize_t n = recv(fds[i].fd, buf, BUFSIZE, 0);

        if (n > 0) {
          send_back_to_client(fds[i].fd, n, buf);
        } else if (n == 0 || (fds[i].revents & (POLLERR | POLLHUP))) {
          close(fds[i].fd);
          fds[i] = fds[nfds - 1];
          nfds--;
          i--;
        }
      }
    }
  }

  // shuts down the listen file descriptor
  close(lfd);
  return 0;
}
