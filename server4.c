// O serviço a ser desenvolvido deve receber requisições com um número de 0 a 9 e responder uma string (mantida em uma tabela)de 128 bytes.
// Esse servidor deve atender requisições sequencialmente, mantendo a conexão estabelecida com um cliente até que este cliente feche a conexão.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define PORT "8080"
#define BACKLOG 10
#define MAXDATASIZE 2

char responseStrings[10][128] = {"Hello", "ola", "aaa", "bbb", "ccc", "ddd", "eee", "fff", "ggg", "hhh"};

void sigchld_handler(int s)
{
  (void)s;

  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void *connection_handler(void *socket_desc)
{
  printf("%d\n", getpid());
  //Get the socket descriptor
  int sock = *(int *)socket_desc;
  int numbytes;
  char buf[MAXDATASIZE];

  
  printf("%d\n", sock);
  //Receive a message from client
  if ((numbytes = read(sock, buf, MAXDATASIZE - 1)) == -1)
  {
    printf("server: client has disconnected\n");
    exit(0);
  }
  printf("a\n");

  buf[numbytes] = '\0';

  char *response = responseStrings[atoi(buf)];

  if (send(sock, response, 128, 0) == -1)
  {
    perror("send");
  }

  return NULL;
}

int main(void)
{
  int status, sockfd, newfd, yes = 1, numbytes;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  struct sigaction sa;
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1)
    {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1)
    {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo);

  if (p == NULL)
  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1)
  {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
    perror("sigaction");
    exit(1);
  }

  while (1)
  {
    printf("Waiting for new connections...\n");

    sin_size = sizeof their_addr;

    while ((newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size) > 0))
    {
      inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s);

      printf("server: got connection from %s\n", s);

      pthread_t handler_thread;

      if (pthread_create(&handler_thread, NULL, connection_handler, &newfd) < 0)
      {
        perror("could not create thread");
        return 1;
      }
    }
  }

  close(sockfd);

  return 0;
}