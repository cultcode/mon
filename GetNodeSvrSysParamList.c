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
#include <curl/curl.h>
#include "Security.h"
#include "SocketHttp.h"
#include "GetNodeSvrSysParamList.h"
#include "cJSON.h"

__attribute__((weak)) int servertimezone=DEFAULT_SERVERTIMEZONE;
__attribute__((weak)) int debugl = DEFAULT_DEBUGL;
static int transfered;

static size_t readfunction( void *ptr, size_t size, size_t nmemb, void *userdata)
{
  int length = -1;
  char * cipher=NULL, content[CONTENT_LEN]={0};
  char *out=NULL;
  char EpochTime[16]={0};
  cJSON *root=NULL;
  struct NodeSvrSysParamList * nsspl = (struct NodeSvrSysParamList *)userdata;
  char buf[128] = {0};

  if(transfered) return 0;

  root=cJSON_CreateObject();

  sprintf(EpochTime,"%lx",nsspl->EpochTime);
  cJSON_AddStringToObject(root,"EpochTime",EpochTime);
  if(!svrversion) {
    cJSON_AddNumberToObject(root,"NodeId",nsspl->NodeId);
  }

  strcpy(content, out=cJSON_PrintUnformatted(root));

  cJSON_Delete(root);

  free(out);

if (debugl >= 1) {
  time_t t=time(NULL);
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] sent     to   %s:\t%s\n",buf,"GetParamList",content);
}
  length = ContentEncode(NODE_3DES_KEY, NODE_3DES_IV, content, &cipher, strlen(content));
  if(length<0){
    fprintf(stderr,"ENDEC ContentEncode() failed");	      
    return CURL_READFUNC_ABORT;
  }

  memcpy(ptr,cipher,length);

  free(cipher);

  transfered=1;

  return length;
}

static size_t writefunction( void *ptr, size_t size, size_t nmemb, void *userdata)
{
  int length = -1;
  char * plain = NULL;
  cJSON *root=NULL, *item=NULL;
  struct NodeSvrSysParamList * nsspl = (struct NodeSvrSysParamList *)userdata;
  char buf[128] = {0};

  if(size*nmemb == 0) return 0;

  length = ContentDecode(NODE_3DES_KEY, NODE_3DES_IV, ptr, &plain, nStripNewLine(ptr, size*nmemb));

  if(length<0) {
    fprintf(stderr,"ENDEC ContentDecode() failed");	      
    return 0;
  }

if (debugl >= 1) {
  time_t t=time(NULL);
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] received from %s:\t%s\n",buf,"GetParamList", plain);
}
  if((root = cJSON_Parse(plain)) == NULL) {
    fprintf(stderr,"JSON cJSON_Parse() failed: %s",cJSON_GetErrorPtr());	      
    return 0;
  }

  item = cJSON_GetObjectItem(root,"Status");
  nsspl->Status = item->valueint;

  item = cJSON_GetObjectItem(root,"StatusDesc");
  strcpy(nsspl->StatusDesc, item->valuestring);

  if(nsspl->Status == SUCCESS) {
    item = cJSON_GetObjectItem(root,"ParmList");
    item = item->child;
    while(item) {
      if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_CollectRateDiskIO")) {
        nsspl->NS_ResMon_CollectRateDiskIO = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      else if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_CollectRateIP")) {
        nsspl->NS_ResMon_CollectRateIP = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      else if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_CollectRateNetFlow")) {
        nsspl->NS_ResMon_CollectRateNetFlow = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      else if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_ReportType")) {
        strcpy(nsspl->NS_ResMon_ReportType , cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      else if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_ConnectTimeOut")) {
        connect_timeout = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      else if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_TransmitTimeOut")) {
        transmit_timeout = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      else if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_ConnectNmsTimeOut")) {
        connect_timeout_nms = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      else if(!strcmp(cJSON_GetObjectItem(item,"ParmName")->valuestring, "NS_ResMon_TransmitNmsTimeOut")) {
        transmit_timeout_nms = atoi(cJSON_GetObjectItem(item,"ParmValue")->valuestring);
      }
      item = item->next;
    }
  }

  cJSON_Delete(root);
  
  if(nsspl->Status == FAIL) {
    fprintf(stderr,"POST FAIL received");	  
    return 0;
  }

  free(plain);
  return size*nmemb;
} 
void GetNodeSvrSysParamList(struct NodeStatus* ns, struct NodeSvrSysParamList* nsspl, char* url)
{
  nsspl->EpochTime = GetLocaltimeSeconds(servertimezone);
  nsspl->NodeId = ns->NodeId;

  CURLcode res;

  char posturl[URL_LEN]={0};
  sprintf(posturl,"%s",url);

  static struct curl_slist *header = NULL;
  if(!header) {
    header = curl_slist_append(header, "Transfer-Encoding: chunked");
    header = curl_slist_append(header, "Connection: keep-alive");
  }

  transfered=0;

  static  CURL *curl=NULL;
  static char error[CURL_ERROR_SIZE]={0};
  if(!curl) {
    curl = curl_easy_init();  
    curl_easy_setopt(curl, CURLOPT_POST, 1);  
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, readfunction);  
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);  
  }

  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, transmit_timeout);

  curl_easy_setopt(curl, CURLOPT_URL, posturl);  
  curl_easy_setopt(curl, CURLOPT_READDATA, nsspl);  
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, nsspl);  

  res = curl_easy_perform(curl);  
  //curl_easy_cleanup(curl);  
  //curl_slist_free_all(header);
  if(CURLE_OK != res) {
    fprintf(stderr, "ERROR: %s\n", error);
    //exit(1);
  }
}

