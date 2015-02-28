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


  if (debugl >= 1) {
    printf("Connecting to server[%s:%hd]......",ip,port);
  }

  if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    perror("connect() The following error occurred");
    exit(1);
  }

  if (debugl >= 1) {
    printf("Connected\n");
  }


  return sockfd;
}

void closeHttp(int sockfd)
{
  int ret=-1;

  if(sockfd == -1) return;

  ret = close(sockfd);

  if(ret < 0) {
    perror("close() The following error occurred");
    exit(1);
  }
}
  
void sendHttp(int* sockfdp, char * url, char * connection, char * input, int encode, char *extra_header)
{
  char sendline[HTTP_LEN]={0};
  char * cipher = NULL;
  char contentlen[CONTENTLEN_LEN] = {0};
  char path[PATH_LEN]={0};
  char host[HOST_LEN] = {0};
  short port=0;
  char  port_char[PORT_LEN] = {0};
  int length = 0;
  int ret=-1;
  char buf[128] = {0};
  time_t t=time(NULL);
  int sockfd = *sockfdp;

  if(sockfd == -1) return;

  ParseUrl(url, NULL, host, &port, path);

  sprintf(port_char,"%hd", port);

  memset(sendline, 0, sizeof(sendline));

//  if(((strlen(plain)/8+1)*8) >= sizeof(cipher)) {
//    fprintf(stderr,"plain is too long to store\n");
//    exit(1);
//  }

if (debugl >= 1) {
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] sent     to   %s:\t%s\n",buf,url,input);
}

  if(encode){
    length = ContentEncode(NODE_3DES_KEY, NODE_3DES_IV, input, &cipher, strlen(input));
  }else{
    if((cipher = malloc(strlen(input)+1)) == NULL) {
      perror("malloc()");
      exit(1);
    }
    strcpy(cipher,input);
    length = strlen(cipher);
  }

  if(length < 0) {
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
  strcat(sendline, HTTP_NEWLINE);

  strcat(sendline, "Host: ");
  strcat(sendline, host);

  if(port) {
    strcat(sendline, ":");
    strcat(sendline, port_char);
  }
  strcat(sendline, HTTP_NEWLINE);

  if(extra_header) {
    strcat(sendline, extra_header);
  }

  strcat(sendline, "Content-Type: ");
  strcat(sendline, "text/plain");
  strcat(sendline, HTTP_NEWLINE);

  strcat(sendline, "Content-Length: ");
  strcat(sendline, contentlen);
  strcat(sendline, HTTP_NEWLINE);

  strcat(sendline, "Connection: ");
  strcat(sendline, connection);
  strcat(sendline, HTTP_NEWLINE);

  strcat(sendline, HTTP_NEWLINE);

  //strcat(sendline, content);

  memcpy(&sendline[strlen(sendline)],cipher,length);

  free(cipher);

//  printf("sendline:\n%s\n", sendline);

if (debugl >= 4) {
  printf("sendline:\n%s\n", sendline);
}

  ret = write(sockfd,sendline,strlen(sendline));
  if (ret < 0) {
    perror("write() The following error occurred");

    if(errno == EPIPE) {
      closeHttp(sockfd);
      *sockfdp = -1;
    }
    else {
      exit(1);
    }
  }else{
if (debugl >= 3) {
    printf("Successfully, %d bytes content has been sent!\n", ret);
}
  }
}

void recvHttp(int* sockfdp, char * url, char* output, int encode)
{
  char recvline[HTTP_LEN] = {0};;
  char *plain = NULL;
  char newline[16]={0};
  char * content=NULL;
  int length=0;
  char buf[128] = {0};
  time_t t=time(NULL);
  int sockfd = *sockfdp;

  if(sockfd == -1) return;

  strcat(newline, HTTP_NEWLINE);
  strcat(newline, HTTP_NEWLINE);

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

if (debugl >= 4) {
  printf("recvline:\n%s\n",recvline);
}

  if(strstr(recvline,"OK") == NULL) {
    printf("wrong HTTP response received\n");

if (debugl < 4) {
    printf("recvline:\n%s\n",recvline);
}
    closeHttp(sockfd);
    *sockfdp = -1;
    
    strcpy(output,"");
  }
  else {

    if((content = strstr(recvline, newline)) == NULL) {
      fprintf(stderr,"strstr failed, string is\n%s\n",recvline);
      exit(1);
    }

    content += 4;

    StripNewLine(content);

    if(encode){
      length = ContentDecode(NODE_3DES_KEY, NODE_3DES_IV, content, &plain, strlen(content));
    }else{
      if((plain = malloc(strlen(content)+1)) == NULL) {
        perror("malloc()");
        exit(1);
      }
      strcpy(plain,content);
      length = strlen(content);
    }

    if(length < 0) {
      fprintf(stderr,"ContentDecode failed\n");
      free(plain);
      exit(1);
    }

    memcpy(output, plain, length);
    output[length] = 0;

    free(plain);
  }

  StripNewLine(output);

if (debugl >= 1) {
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] received from %s:\t%s\n",buf,url, output);
}

  return;
}

