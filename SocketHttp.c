#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <libgen.h>
#include "SocketHttp.h"
#include "NodeResourceStatus.h"
#include "Security.h"

char node_3des_key[KEY_LEN]=DEFAULT_NODE_3DES_KEY;
char node_3des_iv[KEY_LEN]=DEFAULT_NODE_3DES_IV;
__attribute__((weak)) int debugl = DEFAULT_DEBUGL;

/*
 * connect to server ip:port
 */
int isOpen(char * ip, short port, int type) {
  int    sockfd=-1;
  struct sockaddr_in    servaddr={0};

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if(!ip)
    return 0;

  if(!port) 
    return 1;

  if( inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
    return 0;
  }

  if((sockfd = socket(AF_INET, type, 0)) < 0) {
    return 0;
  }

  if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
    if (debugl >= 3) {
      printf("[%s:%hd] is not accessible\n",ip,port);
    }
    close(sockfd);
    return 0;
  }
  else {
    if (debugl >= 3) {
      printf("[%s:%hd] is accessible\n",ip,port);
    }
    close(sockfd);
    return 1;
  }
} 

int createHttp(char * ip, short port, int type, int to)
{
  int    sockfd=-1;
  struct sockaddr_in    servaddr={0};
  struct timeval timeout;

  switch(to) {
    case -1:
      to = 60;
      break;
    case -2:
      break;
    default:
      break;
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  if( inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
    fprintf(stderr,"ERROR: inet_pton() failed when convert ip:[%s]\n",ip);
    exit(1);
  }

  if((sockfd = socket(AF_INET, type, 0)) < 0) {
    perror("socket() The following error occurred");
    exit(1);
  }

  if((type == SOCK_STREAM) && (to >= 0)) {
    timeout.tv_sec = to;
    timeout.tv_usec = 0;

    if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(struct timeval)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval)) == -1) {
      perror("setsockopt");
      exit(1);
    }
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
//    fprintf(stderr,"ERROR: plain is too long to store\n");
//    exit(1);
//  }

if (debugl >= 1) {
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] sent     to   %s:\t%s\n",buf,basename(url),input);
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
    fprintf(stderr,"ERROR: ContentEncode failed\n");
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

  //must getsockopt if tcp/udp is configured
  ret = write(sockfd,sendline,strlen(sendline));
  if (ret < 0) {
    if(errno == EPIPE) {
      closeHttp(sockfd);
      *sockfdp = -1;
    }
    else {
      perror("write() The following error occurred");
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
  time_t t;
  int sockfd = *sockfdp;
  char content_body[CONTENT_LEN] = {0};
  int content_length=0;

  if(sockfd == -1) return;

  strcat(newline, HTTP_NEWLINE);
  strcat(newline, HTTP_NEWLINE);

  memset(recvline, 0, sizeof(recvline));

  length = read(sockfd, recvline, sizeof(recvline)-1);

  if(length == -1) {
    if(errno == EAGAIN) {
      strcpy(output,"");
    }
    else {
      perror("read()");
      exit(1);
    }
  }
  else if(length == 0) {
    closeHttp(sockfd);
    *sockfdp = -1;
    
    strcpy(output,"");
  }
  else if (length == (sizeof(recvline)-1)) {
    fprintf(stderr,"ERROR: http response is too large to store\n");
    exit(1);
  }
  else {

if (debugl >= 4) {
  printf("recvline:\n%s\n",recvline);
}

  if(strstr(recvline,"OK") == NULL) {
if(debugl >= 2) {
    printf("ERROR:non-OK HTTP response received from %s\n",url);
}

//if (debugl < 4) {
//    printf("recvline:\n%s\n",recvline);
//}
//    closeHttp(sockfd);
//    *sockfdp = -1;
    
    strcpy(output,"");
  }
  else {

    if((content = strstr(recvline, newline)) == NULL) {
      fprintf(stderr,"ERROR: strstr failed, string is\n%s\n",recvline);
      exit(1);
    }

    content += 4;

    if(strstr(recvline,"chunked")) {
      content_length = JoinChunk(content,HTTP_NEWLINE,content_body);
      content = content_body;
if (debugl >= 4) {
      printf("\"Transfer-Encoding: chunked\" detected, content body:\n%s\n",content_body);
}
    }
    else {
      strcpy(content_body, content);
      content = content_body;
      content_length = strlen(content);
    }

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
      if(debugl>=2) {
      fprintf(stderr,"ERROR: ContentDecode failed\n");
      }
      output[0] = 0;
      //free(plain);
      //exit(1);
    }
    else {
      memcpy(output, plain, length);
      output[length] = 0;
    }

    free(plain);
  }
}

  StripNewLine(output);

if (debugl >= 1) {
  t=time(NULL);
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] received from %s:\t%s\n",buf,basename(url), output);
}

  return;
}

