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
#include <string.h>
#include "SocketHttp.h"
#include "ReportNodeStatus.h"
#include "cJSON.h"

__attribute__((weak)) int servertimezone=DEFAULT_SERVERTIMEZONE;
__attribute__((weak)) int debugl = DEFAULT_DEBUGL;

char report_type_s[REPORT_TYPE_LEN]="TCP";
static char report_type_s_o[REPORT_TYPE_LEN]={0};
int port_udp=8942;

void ReportNodeStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs, char * url_o)
{
  static int sockfd=-1;
  char content_send[CONTENT_LEN]={0};
  char content[CONTENT_LEN]={0};
  char connection[CONNECTION_LEN] = "Keep-Alive";
  //char connection[CONNECTION_LEN] = "Close";
  //int ret=0;

  char ip[IP_LEN] = {0};
  short port=0;
  char url[URL_LEN]={0};

  char *out=NULL;
  char EpochTime[16]={0};
  cJSON *root=NULL, *item=NULL;

  int  report_type;

  if(!strcasecmp(report_type_s,"TCP")) report_type = SOCK_STREAM;
  else if(!strcasecmp(report_type_s,"UDP")) report_type = SOCK_DGRAM;
  else report_type = SOCK_STREAM;

  InsertPort(url, url_o, report_type==SOCK_STREAM?80:port_udp);

  ParseUrl(url, NULL, ip, &port, NULL);

  //, report_type==SOCK_STREAM?80:port_udp

  //memset(nrs,0,sizeof(struct NodeResourceStatus));

  GetNodeResourceStatus(nsl, nrs);

  nrs->EpochTime = GetLocaltimeSeconds(servertimezone);

  nrs->NodeId = nsl->NodeId;

/*structure http request

{"EpochTime":"97d76a","NodeId":1,"CurrentConn":100,"CurrentBandwidth":54241241,"DiskTotalSpace":64132131634,"DiskFreeSpace":45431,"CpuUsage":60,"MemUsage":40,"WanUsage":60,"LanUsage":40,"IoUsage":15,"LanIpState":1,"WanIpState":1}
*/

//  sprintf(content,
//    "{"
//    "\"EpochTime\":\"%lx\","
//    "\"NodeId\":%d,"
//    "\"CurrentConn\":%lld,"
//    "\"CurrentBandwidth\":%d,"
//    "\"DiskTotalSpace\":%lld,"
//    "\"DiskFreeSpace\":%lld,"
//    "\"CpuUsage\":%d,"
//    "\"MemUsage\":%d,"
//    "\"WanUsage\":%d,"
//    "\"LanUsage\":%d,"
//    "\"IoUsage\":%d,"
//    "\"LanIpState\":%d,"
//    "\"WanIpState\":%d"
//    "}",
//    nrs->EpochTime,
//    nrs->NodeId,
//    nrs->CurrentConn,     
//    nrs->CurrentBandwidth,
//    nrs->DiskTotalSpace,
//    nrs->DiskFreeSpace,
//    (int)nrs->CpuUsage,
//    (int)nrs->MemUsage,
//    (int)nrs->WanUsage,
//    (int)nrs->LanUsage,
//    (int)nrs->IoUsage,
//    nrs->LanIpState,
//    nrs->WanIpState
//  );

  root=cJSON_CreateObject();

  sprintf(EpochTime,"%lx",nrs->EpochTime);
  cJSON_AddStringToObject(root, "EpochTime"         , EpochTime);
  cJSON_AddNumberToObject(root, "NodeId"            , nrs->NodeId);
  cJSON_AddNumberToObject(root, "CurrentConn"       , nrs->CurrentConn);
  cJSON_AddNumberToObject(root, "CurrentBandwidth"  , nrs->CurrentBandwidth);
  cJSON_AddNumberToObject(root, "DiskTotalSpace"    , nrs->DiskTotalSpace);
  cJSON_AddNumberToObject(root, "DiskFreeSpace"     , nrs->DiskFreeSpace);
  cJSON_AddNumberToObject(root, "CpuUsage"          , (int)nrs->CpuUsage);
  cJSON_AddNumberToObject(root, "MemUsage"          , (int)nrs->MemUsage);
  cJSON_AddNumberToObject(root, "WanUsage"          , (int)nrs->WanUsage);
  cJSON_AddNumberToObject(root, "LanUsage"          , (int)nrs->LanUsage);
  cJSON_AddNumberToObject(root, "IoUsage"           , (int)nrs->IoUsage);
  cJSON_AddNumberToObject(root, "LanIpState"        , nrs->LanIpState);
  cJSON_AddNumberToObject(root, "WanIpState"        , nrs->WanIpState);

  strcpy(content, out=cJSON_PrintUnformatted(root));

  cJSON_Delete(root);

  free(out);

  strcpy(content_send, content);

  if(!strlen(report_type_s_o)) {
  }
  else {
    if(!strcasecmp(report_type_s_o, report_type_s)) {
    }
    else{
      closeHttp(sockfd);
      sockfd = -1;
    }
  }
  strcpy(report_type_s_o, report_type_s);

CREATEHTTP:
  if(sockfd == -1) {
    sockfd = createHttp(ip,port,report_type,(report_type==SOCK_STREAM)?-1:-2);
  }

  sendHttp(&sockfd, url, connection, content_send, 1, NULL, (report_type==SOCK_STREAM)?1:0);

  if(sockfd == -1) goto CREATEHTTP;

  memset(content, 0, sizeof(content));

  if(report_type == SOCK_STREAM) {
    recvHttp(&sockfd,url,content,1);
    if(sockfd == -1) goto CREATEHTTP;
  }

/*analyze http content received
{"Status":1,"StatusDesc":"success"}

{"Status":0,"StatusDesc":"CheckFailed"}
*/
  //printf("ReportNodeStatusList() http content received:\n%s\n",content);

//  ret = sscanf(content,
//    "{"
//    "\"Status\":%d,"
//    "\"StatusDesc\":\"%[^\"]\""
//    "}",
//    &nrs->Status,
//    nrs->StatusDesc
//  );

  if(strlen(content)) {
    if((root = cJSON_Parse(content)) == NULL) {
      fprintf(stderr,"Error before: [%s]\n",cJSON_GetErrorPtr());
      exit(1);
    }

    //item = root->child;

    item = cJSON_GetObjectItem(root,"Status");
    nrs->Status = item->valueint;

    item = cJSON_GetObjectItem(root,"StatusDesc");
    strcpy(nrs->StatusDesc, item->valuestring);

    cJSON_Delete(root);
    
if (debugl >= 3) {
    printf("ReportNodeStatus()\n"
      "{"
      "\"Status\":%d,"
      "\"StatusDesc\":\"%s\""
      "}"
      "\n",
      nrs->Status,
      nrs->StatusDesc
    );
}
  }
  else {
  }

  if(!strcasecmp(connection, "Close")){
    closeHttp(sockfd);
    sockfd = -1;
  }

if(debugl >= 2) {
    if(nrs->Status == FAIL) {
      fprintf(stderr,"ERROR: ReportNodeStatus() received FAIL: %s\n", nrs->StatusDesc);
    //  exit(1);
    }
}
}

