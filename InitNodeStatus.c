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

void InitNodeStatus(struct NodeStatus* ns, char* ip, short port)
{
  static int sockfd=-1;
  char content[CONTENT_LEN];
  char contentlen[CONTENTLEN_LEN];
  char connection[CONNECTION_LEN] = "Close";
  struct timeval tv={0};
  int ret=0;

/*structure http request
{"EpochTime":" 97d76a","NodeName":"Imgo-1","Version":"3.0"}
*/

  memset(content, 0, sizeof(content));

  gettimeofday(&tv,NULL);
  ns->EpochTime = tv.tv_sec;

  gethostname(ns->NodeName,NODENAME_LEN);

  sprintf(ns->Version,"%s",VERSION);

  sprintf(contentlen, "%zu", strlen(content));

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

  sendHttp(sockfd, "POST / HTTP/1.1", "www.baidu.com", "text/html", contentlen, connection, content);

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

