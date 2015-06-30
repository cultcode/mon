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
#include "InitNodeStatus.h"
#include "cJSON.h"

int svrversion=DEFAULT_SVRVERSION;
int svrtype=DEFAULT_SVRTYPE;

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
  struct NodeStatus * ns = (struct NodeStatus *)userdata;
  char buf[128] = {0};

  if(transfered) return 0;

  root=cJSON_CreateObject();

  sprintf(EpochTime,"%lx",ns->EpochTime);
  cJSON_AddStringToObject(root,"EpochTime",EpochTime);
  cJSON_AddStringToObject(root,"NodeName",ns->NodeName);
if(svrversion) {
  cJSON_AddStringToObject(root,"Version",ns->Version);
}
if(svrtype) {
  cJSON_AddNumberToObject(root,"SvrType",ns->SvrType);
}

  strcpy(content, out=cJSON_PrintUnformatted(root));

  cJSON_Delete(root);

  free(out);

if (debugl >= 1) {
  time_t t=time(NULL);
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&t));  
  printf("[%s] sent     to   %s:\t%s\n",buf,"InitNodeStatus",content);
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
  struct NodeStatus * ns = (struct NodeStatus *)userdata;
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
  printf("[%s] received from %s:\t%s\n",buf,"InitNodeStatus", plain);
}
  if((root = cJSON_Parse(plain)) == NULL) {
    fprintf(stderr,"JSON cJSON_Parse() failed: %s",cJSON_GetErrorPtr());	      
    return 0;
  }

  item = cJSON_GetObjectItem(root,"Status");
  ns->Status = item->valueint;

  item = cJSON_GetObjectItem(root,"StatusDesc");
  strcpy(ns->StatusDesc, item->valuestring);

  if(ns->Status == SUCCESS) {
    item = cJSON_GetObjectItem(root,"NodeId");
    ns->NodeId = item->valueint;
  }

  cJSON_Delete(root);
  
  if(ns->Status == FAIL) {
    fprintf(stderr,"POST FAIL received");	  
    return 0;
  }

  free(plain);
  return size*nmemb;
}  

void InitNodeStatus(struct NodeStatus* ns, char* url)
{
  memset(ns,0,sizeof(struct NodeStatus));

  ns->EpochTime = GetLocaltimeSeconds(servertimezone);

  gethostname(ns->NodeName,NODENAME_LEN);

  sprintf(ns->Version,"%s",VERSION);
  ns->SvrType = svrtype;

  CURLcode res;

  char posturl[URL_LEN]={0};
  sprintf(posturl,"%s",url);

  static struct curl_slist *header = NULL;
  if(!header) {
    header = curl_slist_append(header, "Transfer-Encoding: chunked");
    header = curl_slist_append(header, "Connection: Close");
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
  curl_easy_setopt(curl, CURLOPT_READDATA, ns);  
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, ns);  

  res = curl_easy_perform(curl);  
  //curl_easy_cleanup(curl);  
  //curl_slist_free_all(header);
  if(CURLE_OK != res) {
    fprintf(stderr, "ERROR: %s\n", error);
    exit(1);
  }
}
