#include <stdio.h>
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
#include "GetNodeStatusList.h"

void GetNodeStatusList(struct NodeStatus* ns, struct NodeStatusList* nsl, char * url)
{
  static int sockfd=-1;
  char content[CONTENT_LEN];
  char connection[CONNECTION_LEN] = "Close";
  struct timeval tv={0};
  int ret=0;

  char ip[IP_LEN] = {0};
  short port=0;

  ParseUrl(url, NULL, ip, &port, NULL);
/*structure http request
{"EpochTime":"97d76a","NodeId ":0}
*/
  memset(content, 0, sizeof(content));

  gettimeofday(&tv,NULL);
  nsl->EpochTime = tv.tv_sec;
  nsl->NodeId = ns->NodeId;

  sprintf(content,
    "EpochTime:%lx,"
    "NodeId:%d",
    nsl->EpochTime,
    nsl->NodeId
  );

  if(sockfd == -1) {
    sockfd = createHttp(ip,port);
  }

  sendHttp(sockfd, url, connection, content);

/*analyze http content received
{"Status":1,"StatusDesc":"success","HomeDir":"x:\Clips","LanIp":"192.168.1.1","WanIp":"10.0.0.1","LanPort":"21","WanPort":80}

{"Status":0,"StatusDesc":"CheckFailed","HomeDir":"",",anIp":"","WanIp":"","LanPort":0,"WanPort":0}
*/
  memset(content, 0, sizeof(content));
  recvHttp(sockfd,content);

  printf("GetNodeStatusList() http content received:\n%s\n",content);

  ret = sscanf(content,
    "Status:%d,"
    "StatusDesc:%[^,],"
    "HomeDir:%[^,],"
    "LanIp:%[^,],"
    "WanIp:%[^,],"
    "LanPort:%hd,"
    "WanPort:%hd",
    &nsl->Status,
    nsl->StatusDesc,
    nsl->HomeDir,
    nsl->LanIp,
    nsl->WanIp,
    &nsl->LanPort,
    &nsl->WanPort
  );

  if(!strcasecmp(connection, "Close")){
    closeHttp(sockfd);
    sockfd = -1;
  }
}

