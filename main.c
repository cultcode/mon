#include <stdio.h>
#include <mntent.h>
#include <stdlib.h>
#include <unistd.h>
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
#include <arpa/inet.h>
#include "NodeResourceStatus.h"
#include "InitNodeStatus.h"
#include "GetNodeSvrSysParamList.h"
#include "GetNodeStatusList.h"
#include "NodeResourceStatus.h"
#include "GetNodeResourceStatus.h"
#include "ReportNodeStatus.h"
#include "SocketHttp.h"
#include "OperateXml.h"
#include "main.h"

//public global variables
int debugl = DEFAULT_DEBUGL;
int servertimezone=DEFAULT_SERVERTIMEZONE;

//private global variables
int refresh_interval=DEFAULT_REFRESH_INTERVAL ; /*time interval to report data*/
int standalone = DEFAULT_STANDALONE ;
int looptimes  = DEFAULT_LOOPTIMES ;

char   HomeDir[HOMEDIR_LEN];
char   LanIp[IP_LEN];
char   WanIp[IP_LEN];
short  LanPort;
short  WanPort;
char   url[4][URL_LEN];
char *SelfName;

int paramlist_interval=0;

static void handler_sigpipe(int signum)
{
  signal(SIGPIPE,handler_sigpipe);
  printf("Caught signal %d\n",signum);
}

int main(int argc, char **argv)
{
  int i=0;
  int ret;
  struct NodeStatus ns = {0};
  struct NodeStatusList nsl = {0};
  struct NodeResourceStatus nrs = {0};
  struct NodeSvrSysParamList nsspl = {0};

  char **options=NULL;
  char ConfigXml[FN_LEN] = {0};
  int argcount=0;
  struct stat laststat={0}, tempstat={0};
  int ArgMode=0;
  double time=0,elapsed=0;

  SelfName=argv[0];

#if 0
  else if((hptr = geturlbyname(host[0])) == NULL) {
    herror("geturlbyname()");
    exit(1);
  }
  else {
    switch(hptr->h_addrtype){
      case AF_INET:
      case AF_INET6:
        pptr=hptr->h_addr_list;
        for(;*pptr!=NULL;pptr++)
          printf("address:%s\n",inet_ntop(hptr->h_addrtype,*pptr,ip,sizeof(ip)));
        break;
      default:
        printf("unknownaddresstype\n");break;
    }
  }
#endif


/********************************************************
 * parse xml
 ********************************************************/
  if((argc == 1) || ((argc==2) &&(argv[1][0] != '-'))) {
    if(argc == 1) {
      strcpy(ConfigXml,argv[0]);
      strcat(ConfigXml,".config");
    }else {
      strcpy(ConfigXml,argv[1]);
    }

    if(stat(ConfigXml, &tempstat) != -1) {
      ArgMode = 1;
      goto GET_ARGUMENTS_XML;
    }
    else {
      fprintf(stderr,"ERROR: Neither cmdline arguments nor .xml provieded\n");
      exit(1);
    }
  }
  else {
    ArgMode = 0;
    ret = ParseOptions(argc, argv);

    if(ret) {
      exit(1);
    }
    goto GET_ARGUMENTS_END;
  }

GET_ARGUMENTS_XML:
  memcpy(&laststat,&tempstat, sizeof(struct stat));

  if((argcount = ReadConfigXml(ConfigXml, &options)) <= 0) {
    //exit(1);
  }
  else {
    ret = ParseOptions(argcount, options);

    for(i=0;i<argcount;i++) {
      free(options[i]);
    }
    free(options);
    options = NULL;

    if(ret) {
      //exit(1);
    }
  }

GET_ARGUMENTS_END:

if(standalone) {
  if(!(strlen(HomeDir) && strlen(WanIp) && strlen(LanIp) && WanPort && LanPort)) {
    printf("Program runs in standalone mode, but not all arguments provided\n");
    exit(1);
  }
  strcpy(nsl.HomeDir,HomeDir);
  strcpy(nsl.WanIp,WanIp);
  strcpy(nsl.LanIp,LanIp);
  nsl.WanPort = WanPort;
  nsl.LanPort = LanPort;
}

/********************************************************
 * Handle SIGPIPE
 ********************************************************/
if(!standalone) {
  signal(SIGPIPE,handler_sigpipe);
}

  signal(SIGUSR1,ReopenLog);

/********************************************************
 * InitNodeStatus
 ********************************************************/
if(!standalone){

  InitNodeStatus(&ns, url[0]);
}


/********************************************************
 * GetNodeStatusList
 ********************************************************/

if(!standalone){

  GetNodeStatusList(&ns, &nsl, url[1]);
}


/********************************************************
 * ReportNodeStatus
 ********************************************************/

while(looptimes) {
  time = doubletime();

  if(standalone) {
    GetNodeResourceStatus(&nsl, &nrs);
  }
  else {
/********************************************************
 * GetNodeSvrSysParamList
 ********************************************************/
    if(strlen(url[3]) && ((GetLocaltimeSeconds(servertimezone) - nsspl.EpochTime) >= paramlist_interval)){
      GetNodeSvrSysParamList(&nsspl, url[3]);

      dsk_average_interval = nsspl.NS_ResMon_CollectRateDiskIO;
if (debugl >= 1) {
      printf("dsk_average_interval: %d\n",dsk_average_interval);
}

      strcpy(report_type_s, nsspl.NS_ResMon_ReportType);
if (debugl >= 1) {
      printf("report type: %s\n",report_type_s);
}
    }

/********************************************************
 * ReportNodeStatus
 ********************************************************/
  if(!strcmp(report_type_s,"TCP")) report_type = SOCK_STREAM;
  else if(!strcmp(report_type_s,"UDP")) report_type = SOCK_DGRAM;
  else {
    fprintf(stderr,"illegal report type=%s\n",report_type_s);
    exit(1);
  }

    ReportNodeStatus(&nsl, &nrs, url[2]);
  }

  if(looptimes == -1 ) {
  }
  else {
    looptimes--;
  }

  if(!looptimes) break;

  elapsed = doubletime()-time;
  if( (int)elapsed >= refresh_interval) {
  }
  else {
    sleep(refresh_interval-(int)elapsed);
  }

  if(ArgMode == 0) continue;

  if(stat(ConfigXml, &tempstat) == -1)  continue;

  if(tempstat.st_mtime != laststat.st_mtime)
  {
    goto GET_ARGUMENTS_XML;
  }

}

return 0;
}
