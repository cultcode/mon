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


int main(int argc, char **argv)
{
  int  i=0,j=0, k=0;;

  int sample_interval=1;  /*time interval to average */
  int refresh_interval=1; /*time interval to report data*/

//struct urlent {
//    char  *h_name;            /* official name of url */
//    char **h_aliases;         /* alias list */
//    int    h_addrtype;        /* url address type */
//    int    h_length;          /* length of address */
//    char **h_addr_list;       /* list of addresses */
//}

  char* const short_options = "i:g:p:s:r:";  
  struct option long_options[] = {  
    { "init",  1,  NULL,  'i'},  
    { "get",  1,  NULL,  'g'},  
    { "report",  1,  NULL,  'p'},  
    { "sample_interval",  1,  NULL,  's'},  
    { "refresh_interval",  1,  NULL,  'r'},  
    {  0,  0,  0,  0},  
  };

#ifdef STANDALONE
#else
  struct NodeStatus ns = {0};
#endif
  struct NodeStatusList nsl = {0};
  struct NodeResourceStatus nrs = {0};

  char  url [3][URL_LEN]  = {{0}};
  char  host[3][HOST_LEN] = {{0}};
  char  ip  [3][IP_LEN]   = {{0}};
  short port[3] = {0};
  int state = 0;

  memset(url,0,sizeof(url));
  memset(host,0,sizeof(host));
  memset(ip,0,sizeof(ip));

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
    case 's':
      sample_interval = atoi(optarg);
      break;
    case 'r':
      refresh_interval = atoi(optarg);
      break;
    }
  }
  printf("init url: %s, get url: %s, report url: %s, refresh interval %d, sample interval:%d second(s)\n",url[0], url[1], url[2], refresh_interval, sample_interval);


  printf("host\t\tip\t\tport\n");
  for(i=0; i<(sizeof(url)/sizeof(url[0])); i++) {
    j = 0;
    k = 0;
    state = 0;
    //http://192.168.8.224:9000/ndas/NodeResMonServerInit
    while(url[i][j]) {
      switch(state) {
        case 0://protocol
          if(url[i][j] == ':') {
            j+=2;
            state = 1;
          }
          break;
        case 1://host
          if(url[i][j] == ':') {
            state = 2;
          }
          else if(url[i][j] == '/') {
            state = 3;
          }
          else {
            host[i][k++] = url[i][j];
          }
          break;
        case 2://port
          if(!port[i]) {
            port[i] = strtol(&url[i][j],NULL,0);
          }

          if(url[i][j] == '/') {
            state = 3;
          }
          break;
        case 3://path
          break;
      }
      j++;
    }
  strcpy(ip[i], host[i]);
  printf("%s\t%s\t%hd\n",host[i], ip[i], port[i]);
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
#ifdef STANDALONE
#else
  InitNodeStatus(&ns, ip[0], port[0]);
  if(ns.Status == FAIL) {
    fprintf(stderr,"InitNodeStatus() received FAIL: %s\n", ns.StatusDesc);
    exit(1);
  }
#endif
  return 0;

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
  GetNodeStatusList(&ns, &nsl, ip[1], port[1]);

  if(nsl.Status == FAIL) {
    fprintf(stderr,"GetNodeStatusList() received FAIL: %s\n", nsl.StatusDesc);
    exit(1);
  }

#endif

/********************************************************
 * ReportNodeStatus
 ********************************************************/

  while(1) {
    ReportNodeStatus(&nsl, &nrs, ip[2], port[2]);

#ifdef STANDALONE
#else
    if(nrs.Status == FAIL) {
      fprintf(stderr,"ReportNodeStatus() received FAIL: %s\n", nrs.StatusDesc);
      exit(1);
    }
#endif

    sleep(refresh_interval);

  }

  return 0;
}
