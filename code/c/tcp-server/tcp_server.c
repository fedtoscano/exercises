#define _DEFAULT_SOURCE
#include <sys/types.h>
#include <arpa/inet.h>   // inet_ntoa, htons, htonl, ntohs
#include <errno.h>
#include <netinet/in.h>  // struct sockaddr_in, INADDR_ANY
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>  // socket, bind, listen, accept, setsockopt, recv, send
#include <unistd.h>      // close

#define PORT 8080
#define BACKLOG 16
#define BUFSIZE 4096

int make_listener(int port) {
   //creating a file descriptor
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1){
    perror("socket");
    return -1;
  }

  struct sockaddr_in s;
  memset(&s, 0, sizeof(s));
  s.sin_family = AF_INET;
  s.sin_addr.s_addr = htonl(INADDR_ANY);
  s.sin_port = htons(port);

  int b = bind(fd, (struct sockaddr*)&s, sizeof(s));
  if(b < 0){
    perror("bind");
    return -1;
  }

  // make a passive socket
  int l = listen(fd, BACKLOG);
  if(l < 0){
    perror("listen");
    return -1;
  }

  return fd;
}

void send_back_to_client(int cfd, ssize_t n, char*  buf){
  ssize_t sent = 0;
  while(sent < n){
    ssize_t s = send(cfd, buf + sent, n - sent, 0);
    if(s < 0){
      perror("send"); 
      return;
    }

    sent += s;
  }
}

void handle_client(int cfd) {
  char buf[BUFSIZE];

  for(;;){
    ssize_t n = recv(cfd, buf, BUFSIZE, 0);
    if(n > 0) send_back_to_client(cfd, n, buf);
    else if(n == 0) return;
    else if(n < 0){
      perror("recv");
      return;
    }
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


  // --- 2. For each connection received, create a CFD (Client File Descriptor)
  for (;;) {
    struct sockaddr_in cli;
    socklen_t len = sizeof cli;

    // Client File Descriptor: specific socket for that particular connection with a client
    int cfd = accept(lfd, (struct sockaddr *)&cli, &len);
    if (cfd < 0) {
      perror("accept");
      continue;
    }
    printf("connessione da %s:%d\n", inet_ntoa(cli.sin_addr),
           ntohs(cli.sin_port));

    handle_client(cfd);
    close(cfd);
    printf("connessione chiusa\n");
  }

  close(lfd);
  return 0;
}
