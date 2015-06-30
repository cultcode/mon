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

