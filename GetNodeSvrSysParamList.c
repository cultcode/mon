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
#include "GetNodeSvrSysParamList.h"
#include "cJSON.h"

__attribute__((weak)) int servertimezone=DEFAULT_SERVERTIMEZONE;
__attribute__((weak)) int debugl = DEFAULT_DEBUGL;


void GetNodeSvrSysParamList(struct NodeSvrSysParamList* nsspl, char* url)
{
  static int sockfd=-1;
  char content_send[CONTENT_LEN]={0};
  char content[CONTENT_LEN]={0};
  //char connection[CONNECTION_LEN] = "Keep-Alive";
  char connection[CONNECTION_LEN] = "Close";
  //int ret=0;
  char ip[IP_LEN] = {0};
  short port=0;

  char *out=NULL;
  char EpochTime[16]={0};
  cJSON *root=NULL, *item=NULL;

  ParseUrl(url,NULL, ip, &port, NULL);
/*structure http request
{"EpochTime":"97d76a"} 
*/
  memset(nsspl,0,sizeof(struct NodeSvrSysParamList));

  nsspl->EpochTime = GetLocaltimeSeconds(servertimezone);

  root=cJSON_CreateObject();

  sprintf(EpochTime,"%lx",nsspl->EpochTime);
  cJSON_AddStringToObject(root,"EpochTime",EpochTime);

  strcpy(content, out=cJSON_PrintUnformatted(root));

  cJSON_Delete(root);

  free(out);

  strcpy(content_send, content);

CREATEHTTP:
  if(sockfd == -1) {
    sockfd = createHttp(ip,port,SOCK_STREAM);
  }

  sendHttp(&sockfd, url, connection, content_send, 1, NULL);

  if(sockfd == -1) goto CREATEHTTP;

  memset(content, 0, sizeof(content));

  recvHttp(&sockfd,url,content,1);

  if(sockfd == -1) goto CREATEHTTP;

/*analyze http content received
 *
{"Status":1,"StatusDesc":"success","ParmList":[{"ParmName":"NS_ResMon_CollectRateDiskIO","ParmValue":"3","},{"ParmName":"NS_ResMon_ReportType","ParmValue":"0"}]}

{"Status":0,"StatusDesc":"CheckFailed",ParmList":"[]}
*/

  if(strlen(content)) {
    if((root = cJSON_Parse(content)) == NULL) {
      fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
      exit(1);
    }

//    item = root->child;

    item = cJSON_GetObjectItem(root,"Status");
    nsspl->Status = item->valueint;
  
    item = cJSON_GetObjectItem(root,"StatusDesc");
    strcpy(nsspl->StatusDesc, item->valuestring);

    if(nsspl->Status == SUCESS) {
      item = cJSON_GetObjectItem(root,"ParmList");
      item = item->child;
      while(item) {
        if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_CollectRateDiskIO")) {
          nsspl->NS_ResMon_CollectRateDiskIO = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
          break;
        }
        item = item->next;
      }
    }

    cJSON_Delete(root);
    
if (debugl >= 3) {
    printf("InitNodeStatus()\n"
      "{"
      "\"Status\":%d,"
      "\"StatusDesc\":\"%s\","
      "\"NS_ResMon_CollectRateDiskIO\":%d"
      "}"
      "\n",
      nsspl->Status,
      nsspl->StatusDesc,
      nsspl->NS_ResMon_CollectRateDiskIO
    );
}
  }
  else {
  }

  if(!strcasecmp(connection, "Close")){
    closeHttp(sockfd);
    sockfd = -1;
  }

  if(nsspl->Status == FAIL) {
    fprintf(stderr,"ERROR: GetNodeSvrSysParamList() received FAIL: %s\n", nsspl->StatusDesc);
    exit(1);
  }

}

