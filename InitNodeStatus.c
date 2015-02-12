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
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include "SocketHttp.h"
#include "InitNodeStatus.h"
#include "cJSON.h"

void InitNodeStatus(struct NodeStatus* ns, char* url)
{
  static int sockfd=-1;
  char content[CONTENT_LEN]={0};
  char connection[CONNECTION_LEN] = "Close";
  //int ret=0;
  char ip[IP_LEN] = {0};
  short port=0;

  char *out=NULL;
  char EpochTime[16]={0};
  cJSON *root=NULL, *item=NULL;

  ParseUrl(url,NULL, ip, &port, NULL);
/*structure http request
{"EpochTime":" 97d76a","NodeName":"Imgo-1","Version":"3.0"}
*/

  ns->EpochTime = GetLocaltimeSeconds();

  gethostname(ns->NodeName,NODENAME_LEN);

  sprintf(ns->Version,"%s",VERSION);

//  sprintf(content,
//    "{"
//    "\"EpochTime\":\"%lx\","
//    "\"NodeName\":\"%s\""
//#if SERVER_VERSION==2
//    ","
//    "\"Version\":\"%s\""
//#endif
//    "}",
//    ns->EpochTime,
//    ns->NodeName
//#if SERVER_VERSION==2
//    ,
//    ns->Version
//#endif
//
//  );

  root=cJSON_CreateObject();

  sprintf(EpochTime,"%lx",ns->EpochTime);
  cJSON_AddStringToObject(root,"EpochTime",EpochTime);
  cJSON_AddStringToObject(root,"NodeName",ns->NodeName);
if(servegoal == 3) {
  cJSON_AddStringToObject(root,"Version",ns->Version);
}

  strcpy(content, out=cJSON_PrintUnformatted(root));

  cJSON_Delete(root);

  free(out);

  if(sockfd == -1) {
    sockfd = createHttp(ip,port,SOCK_STREAM);
  }

  sendHttp(&sockfd, url, connection, content, 1, NULL);

/*analyze http content received
{"Status":1,"StatusDesc":"success","NodeId":1}
{"Status":0,"StatusDesc":"CheckFailed","NodeId ":0}
*/
  memset(content, 0, sizeof(content));
  recvHttp(sockfd,content,1);

//if (debugl >= 3) {
//  printf("InitNodeStatus() http content received:\n%s\n",content);
//}

//  ret = sscanf(content,
//    "{"
//    "\"Status\":%d,"
//    "\"StatusDesc\":\"%[^\"]\","
//    "\"NodeId\":%d"
//    "}",
//    &ns->Status,
//     ns->StatusDesc,
//    &ns->NodeId
//  );

  if(strlen(content)) {
    if((root = cJSON_Parse(content)) == NULL) {
      fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
      exit(1);
    }

    item = root->child;
    ns->Status = item->valueint;

    item = item->next;
    strcpy(ns->StatusDesc, item->valuestring);

    item = item->next;
    ns->NodeId = item->valueint;

    cJSON_Delete(root);
    
if (debugl >= 3) {
    printf("InitNodeStatus()\n"
      "{"
      "\"Status\":%d,"
      "\"StatusDesc\":\"%s\","
      "\"NodeId\":%d"
      "}"
      "\n",
      ns->Status,
      ns->StatusDesc,
      ns->NodeId
    );
}
  }
  else {
  }

  if(!strcasecmp(connection, "Close")){
    closeHttp(sockfd);
    sockfd = -1;
  }

if(!standalone) {
  if(ns->Status == FAIL) {
    fprintf(stderr,"InitNodeStatus() received FAIL: %s\n", ns->StatusDesc);
    exit(1);
  }
}

}

