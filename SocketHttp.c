#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include "SocketHttp.h"
//#include <openssl/des.h>

/*
 * connect to server ip:port
 */
int createHttp(char * ip, short port)
{
  int    sockfd=-1;
  struct sockaddr_in    servaddr={0};

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if( inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
    perror("inet_pton() The following error occurred");
    exit(1);
  }

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket() The following error occurred");
    exit(1);
  }

  if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    perror("connect() The following error occurred");
    exit(1);
  }

  return sockfd;
}

void closeHttp(int sockfd)
{
  int ret=-1;

  ret = close(sockfd);

  if(ret < 0) {
    perror("close() The following error occurred");
    exit(1);
  }
}
  
void sendHttp(int sockfd,char * request_line, char * host, char * content_type, char * content_len, char * connection, char * content)
{
  char sendline[HTTP_LEN];
  char newline[]={'\r','\n','\0'};
  int ret=-1;

  memset(sendline, 0, sizeof(sendline));

  strcat(sendline, request_line);
  strcat(sendline, newline);

  strcat(sendline, "Host: ");
  strcat(sendline, host);
  strcat(sendline, newline);

  strcat(sendline, "Content-Type: ");
  strcat(sendline, content_type);
  strcat(sendline, newline);

  strcat(sendline, "Content-Length: ");
  strcat(sendline, content_len);
  strcat(sendline, newline);

  strcat(sendline, "Connection: ");
  strcat(sendline, connection);
  strcat(sendline, newline);

  strcat(sendline, newline);

  strcat(sendline, content);

  printf("sendline:\n%s\n", sendline);

  ret = write(sockfd,sendline,strlen(sendline));
  if (ret < 0) {
          perror("write() The following error occurred");
          exit(1);
  }else{
          printf("Successfully, %d bytes content has been sent!\n", ret);
  }
}

void recvHttp(int sockfd, char* str)
{
  char recvline[HTTP_LEN];
  char newline[]={'\r','\n','\r','\n','\0'};
  char * content=NULL;
  int length=0;

  memset(recvline, 0, sizeof(recvline));

  length = read(sockfd, recvline, sizeof(recvline)-1);

  printf("recvline:\n%s\n",recvline);


  if (length == (sizeof(recvline)-1)) {
    fprintf(stderr,"http response is too large to store\n");
    exit(1);
  }

  if((content = strstr(recvline, newline)) == NULL) {
    perror("strstr(), The following error occurred");
    exit(1);
  }

  content += 4;

  strcpy(str, content);

  return;
}

