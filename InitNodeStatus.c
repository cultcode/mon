#include <stdio.h>
#include <mntent.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include "SocketHttp.h"
#include "InitNodeStatus.h"

void InitNodeStatus(struct NodeStatus* ns, char* url)
{
  static int sockfd=-1;
  char content[CONTENT_LEN]={0};
  char connection[CONNECTION_LEN] = "Close";
  struct timeval tv={0};
  int ret=0;
  char ip[IP_LEN] = {0};
  short port=0;

  ParseUrl(url,NULL, ip, &port, NULL);
/*structure http request
{"EpochTime":" 97d76a","NodeName":"Imgo-1","Version":"3.0"}
*/

  gettimeofday(&tv,NULL);
  ns->EpochTime = tv.tv_sec;

  gethostname(ns->NodeName,NODENAME_LEN);

  sprintf(ns->Version,"%s",VERSION);

  sprintf(content,
    "EpochTime:%lx,"
    "NodeName:%s,"
    "Version:%s",
    ns->EpochTime,
    ns->NodeName,
    ns->Version
  );

  if(sockfd == -1) {
    sockfd = createHttp(ip,port);
  }

  sendHttp(sockfd, url, connection, content);

/*analyze http content received
{"Status":1,"StatusDesc":"success","NodeId":1}
{"Status":0,"StatusDesc":"CheckFailed","NodeId ":0}
*/
  memset(content, 0, sizeof(content));
  recvHttp(sockfd,content);

  printf("InitNodeStatus() http content received:\n%s\n",content);

  ret = sscanf(content,
    "Status:%d,"
    "StatusDesc:%[^,],"
    "NodeId:%d",
    &ns->Status,
     ns->StatusDesc,
    &ns->NodeId
  );

  if(!strcasecmp(connection, "Close")){
    closeHttp(sockfd);
    sockfd = -1;
  }
}

