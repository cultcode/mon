#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <time.h>
#include "SocketHttp.h"
#include "NodeStatus.h"
#include "Security.h"

/*
 * connect to server ip:port
 */
int createHttp(char * ip, short port, int type)
{
  int    sockfd=-1;
  struct sockaddr_in    servaddr={0};

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if( inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
    fprintf(stderr,"inet_pton() failed when convert ip:[%s]\n",ip);
    exit(1);
  }

  if((sockfd = socket(AF_INET, type, 0)) < 0) {
    perror("socket() The following error occurred");
    exit(1);
  }


  if (debugl >= 3) {
    printf("Connecting to server...\n");
  }

  if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    perror("connect() The following error occurred");
    exit(1);
  }

  if (debugl >= 3) {
    printf("Connected\n");
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
  
void sendHttp(int sockfd,char * url, char * connection, char * input)
{
  char sendline[HTTP_LEN]={0};
  char * cipher = NULL;
  char contentlen[CONTENTLEN_LEN] = {0};
  char newline[]={'\r','\n','\0'};
  char path[PATH_LEN]={0};
  char host[HOST_LEN] = {0};
  short port=0;
  char  port_char[PORT_LEN] = {0};
  int length = 0;
  int ret=-1;
  char buf[128] = {0};
  time_t t=time(NULL);

  ParseUrl(url, NULL, host, &port, path);

  sprintf(port_char,"%hd", port);

  memset(sendline, 0, sizeof(sendline));

//  if(((strlen(plain)/8+1)*8) >= sizeof(cipher)) {
//    fprintf(stderr,"plain is too long to store\n");
//    exit(1);
//  }

if (debugl >= 1) {
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] sent:\t%s\n",buf,input);
}

  length = ContentEncode(NODE_3DES_KEY, NODE_3DES_IV, input, &cipher, strlen(input));

  if(length <= 0) {
    fprintf(stderr,"ContentEncode failed\n");
    free(cipher);
    exit(1);
  }

  //memset(plain, 0, strlen(plain));
  //call_cbc(content, plain, DES_DECRYPT);
  //printf("self test: plain is \n%s\n",plain);

  sprintf(contentlen, "%d", length);

  memset(sendline, 0, sizeof(sendline));

  strcat(sendline, "POST ");
  strcat(sendline, path);
  strcat(sendline, " HTTP/1.1");
  strcat(sendline, newline);

  strcat(sendline, "Host: ");
  strcat(sendline, host);

  if(port) {
    strcat(sendline, ":");
    strcat(sendline, port_char);
  }
  strcat(sendline, newline);

  strcat(sendline, "Content-Type: ");
  strcat(sendline, "text/plain");
  strcat(sendline, newline);

  strcat(sendline, "Content-Length: ");
  strcat(sendline, contentlen);
  strcat(sendline, newline);

  strcat(sendline, "Connection: ");
  strcat(sendline, connection);
  strcat(sendline, newline);

  strcat(sendline, newline);

  //strcat(sendline, content);

  memcpy(&sendline[strlen(sendline)],cipher,length);

  free(cipher);

//  printf("sendline:\n%s\n", sendline);

if (debugl >= 3) {
  printf("sendline:\n%s\n", sendline);
}

  ret = write(sockfd,sendline,strlen(sendline));
  if (ret < 0) {
          perror("write() The following error occurred");
          exit(1);
  }else{
if (debugl >= 3) {
          printf("Successfully, %d bytes content has been sent!\n", ret);
}
  }
}

void recvHttp(int sockfd, char* output)
{
  char recvline[HTTP_LEN] = {0};;
  char *plain = NULL;
  char newline[]={'\r','\n','\r','\n','\0'};
  char * content=NULL;
  int length=0;
  char buf[128] = {0};
  time_t t=time(NULL);

  memset(recvline, 0, sizeof(recvline));

  length = read(sockfd, recvline, sizeof(recvline)-1);

  if(length == -1) {
    perror("read()");
    exit(1);
  }

  if (length == (sizeof(recvline)-1)) {
    fprintf(stderr,"http response is too large to store\n");
    exit(1);
  }

if (debugl >= 3) {
  printf("recvline:\n%s\n",recvline);
}

  if((content = strstr(recvline, newline)) == NULL) {
    fprintf(stderr,"strstr failed, string is\n%s\n",recvline);
    exit(1);
  }

  content += 4;

  length = ContentDecode(NODE_3DES_KEY, NODE_3DES_IV, content, &plain, strlen(content));

  memcpy(output, plain, length);
  output[length] = 0;

if (debugl >= 1) {
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] received:\t%s\n",buf, output);
}

  free(plain);

  return;
}

