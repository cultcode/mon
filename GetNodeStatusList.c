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
#include "cJSON.h"

void GetNodeStatusList(struct NodeStatus* ns, struct NodeStatusList* nsl, char * url)
{
  static int sockfd=-1;
  char content[CONTENT_LEN];
  char connection[CONNECTION_LEN] = "Close";
  //int ret=0;

  char ip[IP_LEN] = {0};
  short port=0;

  char *out=NULL;
  char EpochTime[16]={0};
  cJSON *root=NULL, *item=NULL;

  ParseUrl(url, NULL, ip, &port, NULL);
/*structure http request
{"EpochTime":"97d76a","NodeId ":0}
*/
  memset(content, 0, sizeof(content));

  nsl->EpochTime = GetLocaltimeSeconds();
  nsl->NodeId = ns->NodeId;

//  sprintf(content,
//    "{"
//    "\"EpochTime\":\"%lx\","
//    "\"NodeId\":%d"
//    "}",
//    nsl->EpochTime,
//    nsl->NodeId
//  );

  root=cJSON_CreateObject();

  sprintf(EpochTime,"%lx",nsl->EpochTime);
  cJSON_AddStringToObject(root,"EpochTime",EpochTime);
  cJSON_AddNumberToObject(root,"NodeId",nsl->NodeId);

  strcpy(content, out=cJSON_PrintUnformatted(root));

  cJSON_Delete(root);

  free(out);

  if(sockfd == -1) {
    sockfd = createHttp(ip,port,SOCK_STREAM);
  }

  sendHttp(sockfd, url, connection, content, 1, NULL);

/*analyze http content received
{"Status":1,"StatusDesc":"success","HomeDir":"x:\Clips","LanIp":"192.168.1.1","WanIp":"10.0.0.1","LanPort":"21","WanPort":80}

{"Status":0,"StatusDesc":"CheckFailed","HomeDir":"",",anIp":"","WanIp":"","LanPort":0,"WanPort":0}
*/
  memset(content, 0, sizeof(content));
  recvHttp(sockfd,content, 1);

//if (debugl >= 3) {
//  printf("GetNodeStatusList() http content received:\n%s\n",content);
//}

//  ret = sscanf(content,
//    "{"
//    "\"Status\":%d,"
//    "\"StatusDesc\":\"%[^\"]\","
//    "\"HomeDir\":\"%[^\"]\","
//    "\"LanIp\":\"%[^\"]\","
//    "\"WanIp\":\"%[^\"]\","
//    "\"LanPort\":%hd,"
//    "\"WanPort\":%hd"
//    "}",
//    &nsl->Status,
//    nsl->StatusDesc,
//    nsl->HomeDir,
//    nsl->LanIp,
//    nsl->WanIp,
//    &nsl->LanPort,
//    &nsl->WanPort
//  );

  if(strlen(content)) {
    if((root = cJSON_Parse(content)) == NULL) {
      fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
      exit(1);
    }

    item = root->child;
    nsl->Status = item->valueint;

    item = item->next;
    strcpy(nsl->StatusDesc, item->valuestring);

    item = item->next;
    strcpy(nsl->HomeDir, item->valuestring);

    item = item->next;
    strcpy( nsl->LanIp, item->valuestring);

    item = item->next;
    strcpy( nsl->WanIp, item->valuestring);

    item = item->next;
    nsl->LanPort = item->valueint;

    item = item->next;
    nsl->WanPort = item->valueint;

    cJSON_Delete(root);

if (debugl >= 3) {
    printf("GetNodeStatusList()\n"
      "{"
      "\"Status\":%d,"
      "\"StatusDesc\":\"%s\","
      "\"HomeDir\":\"%s\","
      "\"LanIp\":\"%s\","
      "\"WanIp\":\"%s\","
      "\"LanPort\":%hd,"
      "\"WanPort\":%hd"
      "}"
      "\n",
      nsl->Status,
      nsl->StatusDesc,
      nsl->HomeDir,
      nsl->LanIp,
      nsl->WanIp,
      nsl->LanPort,
      nsl->WanPort
    );
}
  }
  else {
  }

  if(!strcasecmp(connection, "Close")){
    closeHttp(sockfd);
    sockfd = -1;
  }

if(! standalone) {
  if(nsl->Status == FAIL) {
    fprintf(stderr,"GetNodeStatusList() received FAIL: %s\n", nsl->StatusDesc);
    exit(1);
  }
}

}

