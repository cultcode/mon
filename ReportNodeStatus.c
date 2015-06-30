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
#include <curl/curl.h>
#include "Security.h"
#include <netdb.h>
#include <string.h>
#include "SocketHttp.h"
#include "ReportNodeStatus.h"
#include "cJSON.h"

__attribute__((weak)) int servertimezone=DEFAULT_SERVERTIMEZONE;
__attribute__((weak)) int debugl = DEFAULT_DEBUGL;
static int transfered;

char report_type_s[REPORT_TYPE_LEN]="TCP";
int port_udp=8942;
int port_tcp=80;

static size_t readfunction( void *ptr, size_t size, size_t nmemb, void *userdata)
{
  int length = -1;
  char * cipher=NULL, content[CONTENT_LEN]={0};
  char *out=NULL;
  char EpochTime[16]={0};
  cJSON *root=NULL;
  struct NodeResourceStatus * nrs = (struct NodeResourceStatus *)userdata;
  char buf[128] = {0};

  if(transfered) return 0;

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

if (debugl >= 1) {
  time_t t=time(NULL);
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] sent     to   %s:\t%s\n",buf,"ReportNodeStatus",content);
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
  struct NodeResourceStatus * nrs = (struct NodeResourceStatus *)userdata;
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
  printf("[%s] received from %s:\t%s\n",buf,"ReportNodeStatus", plain);
}
  if((root = cJSON_Parse(plain)) == NULL) {
    fprintf(stderr,"JSON cJSON_Parse() failed: %s",cJSON_GetErrorPtr());	      
    return 0;
  }

  item = cJSON_GetObjectItem(root,"Status");
  nrs->Status = item->valueint;

  item = cJSON_GetObjectItem(root,"StatusDesc");
  strcpy(nrs->StatusDesc, item->valuestring);

  cJSON_Delete(root);
  
  if(nrs->Status == FAIL) {
    fprintf(stderr,"POST FAIL received");	  
    return 0;
  }

  free(plain);
  return size*nmemb;
}  

void ReportNodeStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs, char * url)
{
  GetNodeResourceStatus(nsl, nrs);

  nrs->EpochTime = GetLocaltimeSeconds(servertimezone);

  nrs->NodeId = nsl->NodeId;

  int  report_type;

  if(!strcasecmp(report_type_s,"TCP") || !strcasecmp(report_type_s,"0")) report_type = SOCK_STREAM;
  else if(!strcasecmp(report_type_s,"UDP") || !strcasecmp(report_type_s,"1")) report_type = SOCK_DGRAM;
  else report_type = SOCK_STREAM;

if(report_type == SOCK_DGRAM) {
  static int    sockfd=0;
  if(!sockfd){
    if((sockfd = socket(AF_INET, report_type, 0)) < 0) {
      perror("ERROR socket()");
      exit(1);
    }
  }

  struct hostent        *he;
  struct sockaddr_in  server;
  socklen_t addrlen = sizeof(struct sockaddr_in);

  /* resolve hostname */
  if ( (he = gethostbyname(url) ) == NULL ) {
      perror("ERROR gethostbyname()");
      exit(1); /* error */
  }

  /* copy the network address to sockaddr_in structure */
  memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);
  server.sin_family = AF_INET;
  server.sin_port = htons(port_udp);

  char content[CONTENT_LEN] = {0};
  int len = readfunction(content,CONTENT_LEN,1, nrs);

  sendto(sockfd,content, len, 0, (struct sockaddr *)(&server), addrlen);
  return;
}
  //memset(nrs,0,sizeof(struct NodeResourceStatus));

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
  curl_easy_setopt(curl, CURLOPT_READDATA, nrs);  
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, nrs);  

  res = curl_easy_perform(curl);  
  //curl_easy_cleanup(curl);  
  //curl_slist_free_all(header);
  if(CURLE_OK != res) {
    fprintf(stderr, "ERROR: %s\n", error);
    //exit(1);
  }
}

