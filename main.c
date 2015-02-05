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

  char* const short_options = "i:g:p:r:c:m:d:n:";  
  struct option long_options[] = {  
    { "init",  1,  NULL,  'i'},  
    { "get",  1,  NULL,  'g'},  
    { "report",  1,  NULL,  'p'},  
    { "refresh_interval",  1,  NULL,  'r'},  
    { "cpu_average_interval",  1,  NULL,  'c'},  
    { "mem_average_interval",  1,  NULL,  'm'},  
    { "dsk_average_interval", 1,  NULL,  'd'},  
    { "net_average_interval",  1,  NULL,  'n'},  
    {  0,  0,  0,  0},  
  };

#ifdef STANDALONE
#else
  struct NodeStatus ns = {0};
#endif
  struct NodeStatusList nsl = {0};
  struct NodeResourceStatus nrs = {0};

  char  url [3][URL_LEN]  = {{0}};

  memset(url,0,sizeof(url));

/********************************************************
 * get input variables
 ********************************************************/
  while ( -1 != (i = getopt_long(argc, argv, short_options, long_options, NULL))) {
    switch (i) {
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
    }
  }
  printf("init url: %s\n, get url: %s\n, report url: %s\n, refresh interval %d\n, cpu_average_interval %d\n, mem_average_interval %d\n, dsk_average_interval %d\n, net_average_interval %d\n",url[0], url[1], url[2], refresh_interval, cpu_average_interval, mem_average_interval, dsk_average_interval, net_average_interval);


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
#ifdef STANDALONE
#else
  InitNodeStatus(&ns, url[0]);
#endif


/********************************************************
 * GetNodeStatusList
 ********************************************************/

#ifdef STANDALONE
  strcpy(nsl.WanIp, "192.168.8.72");
  strcpy(nsl.LanIp, "192.168.8.72");
  nsl.WanPort = 22;
  nsl.LanPort = 22;
  strcpy(nsl.HomeDir, "/");

#else

  GetNodeStatusList(&ns, &nsl, url[1]);
#endif


/********************************************************
 * ReportNodeStatus
 ********************************************************/

  while(1) {
    ReportNodeStatus(&nsl, &nrs, url[2]);

    sleep(refresh_interval);

  }

  return 0;
}
