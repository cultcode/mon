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
#include <arpa/inet.h>
#include "NodeStatus.h"
#include "InitNodeStatus.h"
#include "GetNodeStatusList.h"
#include "NodeResourceStatus.h"
#include "ReportNodeStatus.h"
#include "SocketHttp.h"

int cpu_average_interval=1;  /*time interval to average */
int mem_average_interval=1;  /*time interval to average */
int dsk_average_interval=1;  /*time interval to average */
int net_average_interval=1;  /*time interval to average */
int standalone = 0;
int debugl = 1;
int servertimezone = 8;

int main(int argc, char **argv)
{
  int  i=0;

  int refresh_interval=1; /*time interval to report data*/

//struct urlent {
//    char  *h_name;            /* official name of url */
//    char **h_aliases;         /* alias list */
//    int    h_addrtype;        /* url address type */
//    int    h_length;          /* length of address */
//    char **h_addr_list;       /* list of addresses */
//}

  char* const short_options = "a:b:i:g:p:r:c:m:d:n:w:l:h:z:";  
  struct option long_options[] = {  
    { "standalone",  1,  NULL,  'a'},  
    { "debugl",  1,  NULL,  'b'},  
    { "init",  1,  NULL,  'i'},  
    { "get",  1,  NULL,  'g'},  
    { "report",  1,  NULL,  'p'},  
    { "refresh_interval",  1,  NULL,  'r'},  
    { "cpu_average_interval",  1,  NULL,  'c'},  
    { "mem_average_interval",  1,  NULL,  'm'},  
    { "dsk_average_interval", 1,  NULL,  'd'},  
    { "net_average_interval",  1,  NULL,  'n'},  
    { "wanip",  1,  NULL,  'w'},  
    { "lanip",  1,  NULL,  'l'},  
    { "homedir",  1,  NULL,  'h'},  
    { "zone",  1,  NULL,  'z'},  
    {  0,  0,  0,  0},  
  };

  struct NodeStatus ns = {0};
  struct NodeStatusList nsl = {0};
  struct NodeResourceStatus nrs = {0};

  char  url [3][URL_LEN]  = {{0}};

  memset(url,0,sizeof(url));

/********************************************************
 * get input variables
 ********************************************************/
  while ( -1 != (i = getopt_long(argc, argv, short_options, long_options, NULL))) {
    switch (i) {
    case 'a':
      standalone = atoi(optarg);
      break;
    case 'b':
      debugl = atoi(optarg);
      break;
    case 'i':
      strcpy(url[0], optarg);
      break;
    case 'g':
      strcpy(url[1], optarg);
      break;
    case 'p':
      strcpy(url[2], optarg);
      break;
    case 'r':
      refresh_interval = atoi(optarg);
      break;
    case 'c':
      cpu_average_interval = atoi(optarg);
      break;
    case 'm':
      mem_average_interval = atoi(optarg);
      break;
    case 'd':
      dsk_average_interval = atoi(optarg);
      break;
    case 'n':
      net_average_interval = atoi(optarg);
      break;
    case 'w':
      standalone  = 1;
      ParseUrl(optarg,NULL,nsl.WanIp,&nsl.WanPort,NULL);
      break;
    case 'l':
      standalone  = 1;
      ParseUrl(optarg,NULL,nsl.LanIp,&nsl.LanPort,NULL);
      break;
    case 'h':
      standalone  = 1;
      strcpy(nsl.HomeDir, optarg);
      break;
    case 'z':
      servertimezone  = 8;
      strcpy(nsl.HomeDir, optarg);
      break;
    }
  }

if (debugl >= 1) {
  printf("debugl: %d\ninit url: %s\nget url: %s\nreport url: %s\nrefresh interval %d\ncpu_average_interval %d\nmem_average_interval %d\ndsk_average_interval %d\nnet_average_interval %d\nwanip:%s, wanport %hd, lanip %s, lanport %hd, homedir %s\n",debugl, url[0], url[1], url[2], refresh_interval, cpu_average_interval, mem_average_interval, dsk_average_interval, net_average_interval,nsl.WanIp,nsl.WanPort,nsl.LanIp,nsl.LanPort,nsl.HomeDir);
}

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

  while(1) {
    ReportNodeStatus(&nsl, &nrs, url[2]);

    sleep(refresh_interval);

  }

  return 0;
}
