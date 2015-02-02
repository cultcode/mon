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
#include "NodeStatus.h"
#include "InitNodeStatus.h"
#include "GetNodeStatusList.h"
#include "NodeResourceStatus.h"
#include "ReportNodeStatus.h"
#include "SocketHttp.h"


int main(int argc, char **argv)
{
  int  i=0;

  int sample_interval=1;  /*time interval to average */
  int refresh_interval=1; /*time interval to report data*/

  char* const short_options = "a:b:c:d:e:f:s:r:";  
  struct option long_options[] = {  
    { "ipinit",  1,  NULL,  'a'},  
    { "portinit",  1,  NULL,  'b'},  
    { "ipget",  1,  NULL,  'c'},  
    { "portget",  1,  NULL,  'd'},  
    { "ipreport",  1,  NULL,  'e'},  
    { "portreport",  1,  NULL,  'f'},  
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

  char ip[3][IP_LEN]={0};

  short port[3] = {80,80,80};

  memset(ip,0,3*IP_LEN);

/********************************************************
 * get input variables
 ********************************************************/
  while ( -1 != (i = getopt_long(argc, argv, short_options, long_options, NULL))) {
    switch (i) {
    case 'a':
      strcpy(ip[0], optarg);
      break;
    case 'b':
      port[0] = atoi(optarg);
      break;
    case 'c':
      strcpy(ip[1], optarg);
      break;
    case 'd':
      port[1] = atoi(optarg);
      break;
    case 'e':
      strcpy(ip[2], optarg);
      break;
    case 'f':
      port[2] = atoi(optarg);
      break;
    case 's':
      sample_interval = atoi(optarg);
      break;
    case 'r':
      refresh_interval = atoi(optarg);
      break;
    }
  }
  printf("ip %s, port %hd, refresh interval %d, sample interval:%d second(s)\n",ip, port, refresh_interval,sample_interval);

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
