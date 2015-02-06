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
#include "SocketHttp.h"
#include "ReportNodeStatus.h"
#include "NodeResourceStatus.h"

void GetNetworkConcernedStatus(struct net_data *data, char * ip, short port, float * usage, int * ipstate, int * bandwidth, long long * cons) {
  int i=0;
  struct net_param *pa=NULL;

  pa=(struct net_param *)&data->net_param;

  for(i=0; i<data->nets; i++) {
    if(!strcmp(ip, pa[i].ip)) {
      *usage   = pa[i].usage;
      *ipstate =(pa[i].flags & IFF_RUNNING) >0;
      if(bandwidth != NULL) {
        *bandwidth = (/*pa[i].speed_ib +*/ pa[i].speed_ob)*8;
      }
      break;
    }
  }
  if(i >= data->nets) {
    fprintf(stderr,"ERROR: can't find ip %s from getifaddrs()\n",ip);
    exit(1);
  }

  if(cons != NULL) {
    *cons = GetCurrentConn(ip, port);
  }
}

void GetFsDiskConcernedState(struct dsk_data * data, char *HomeDir, long long *DiskTotalSpace, long long *DiskFreeSpace, float* IoUsage)
{
  int i=0;
  for(i = 0; i<data->jfses; i++) {
    if(!strcmp(HomeDir, data->jfs[i].name)) {
      *DiskTotalSpace = data->jfs[i].size*1024*1024;
      *DiskFreeSpace  = data->jfs[i].free*1024*1024;
      *IoUsage        = data->jfs[i].io;
      break;
    }
  }
  if(i >= data->jfses) {
    *DiskTotalSpace = 0;
    *DiskFreeSpace  = 0;
    *IoUsage        = 0;
if (debugl >= 2) {
    fprintf(stderr,"WARNING: can't find directory %s from fstatfs()\n",HomeDir);
//#elif DEBUGL <= 1
//    fprintf(stderr,"ERROR: can't find directory %s from fstatfs()\n",HomeDir);
//    exit(1);
}
  }
}

void GetCpuConcernedState(struct cpu_data *data, float * usage) {
  struct cpu_param  *pa = NULL;

  pa=(struct cpu_param *)&data->cpu_param;

  *usage = pa->total_usage;
}

void GetMemConcernedState(struct mem_data *data, float * usage) {
  struct mem_param  *pa = NULL;

  pa=(struct mem_param *)&data->mem_param;

  *usage = pa->usage;
}

void ReportNodeStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs, char * url)
{
  static int sockfd=-1;
  char content[CONTENT_LEN];
  char connection[CONNECTION_LEN] = "Keep-Alive";
  //char connection[CONNECTION_LEN] = "Close";
  int ret=0;

  static int first_time=1;
  static struct proc proc[P_NUMBER] = {{0}};
  static struct net_data net_data = {0};
  static struct dsk_data dsk_data = {0};
  static struct cpu_data cpu_data = {0};
  static struct mem_data mem_data = {0};

  static char ip[IP_LEN] = {0};
  static short port=0;

  if(first_time) {
    first_time = 0;

    ParseUrl(url, NULL, ip, &port, NULL);
    //memset(proc,0,P_NUMBER*sizeof(struct proc));

    cpu_data.p = &cpu_data.cpu_stats[0];
    cpu_data.q = &cpu_data.cpu_stats[1];

    dsk_data.p = &dsk_data.dk[0];
    dsk_data.q = &dsk_data.dk[1];

    mem_data.p = &mem_data.mem_stats[0];
    mem_data.q = &mem_data.mem_stats[1];

    net_data.p = &net_data.net_stats[0];
    net_data.q = &net_data.net_stats[1];
  }

/*structure http request

{"EpochTime":"97d76a","NodeId":1,"CurrentConn":100,"CurrentBandwidth":54241241,"DiskTotalSpace":64132131634,"DiskFreeSpace":45431,"CpuUsage":60,"MemUsage":40,"WanUsage":60,"LanUsage":40,"IoUsage":15,"LanIpState":1,"WanIpState":1}
*/

  memset(content, 0, sizeof(content));

if(debugl >= 3) {
  printf("====================================================================================\n");
  printf("===========================   ReportNodeStatus() START   ===========================\n");
  printf("====================================================================================\n");
}
  proc_init(proc);

  nrs->EpochTime = GetLocaltimeSeconds();

  nrs->NodeId = nsl->NodeId;


  //Networks
  if((doubletime() - net_data.p->time) >= net_average_interval) {
    GetNetworkState(&net_data);
    GetNetworkConcernedStatus(&net_data, nsl->WanIp, nsl->WanPort, &nrs->WanUsage, &nrs->WanIpState, &nrs->CurrentBandwidth, &nrs->CurrentConn);
    GetNetworkConcernedStatus(&net_data, nsl->LanIp, nsl->LanPort, &nrs->LanUsage, &nrs->LanIpState, NULL, NULL);
  }

  //Disk & Filesystem
  if((doubletime() - dsk_data.p->time) >= dsk_average_interval) {
    GetDiskState(&dsk_data);
    GetFsDiskConcernedState(&dsk_data, nsl->HomeDir, &nrs->DiskTotalSpace, &nrs->DiskFreeSpace, &nrs->IoUsage);
  }

  //Cpu
  if((doubletime() - cpu_data.p->time) >= cpu_average_interval) {
    GetCpuState(&cpu_data, proc);
    GetCpuConcernedState(&cpu_data, &nrs->CpuUsage);
  }

  //Mem
  if((doubletime() - mem_data.p->time) >= mem_average_interval) {
    GetMemState(&mem_data, proc);
    GetMemConcernedState(&mem_data, &nrs->MemUsage);
  }

  sprintf(content,
    "{"
    "\"EpochTime\":\"%lx\","
    "\"NodeId\":%d,"
    "\"CurrentConn\":%lld,"
    "\"CurrentBandwidth\":%d,"
    "\"DiskTotalSpace\":%lld,"
    "\"DiskFreeSpace\":%lld,"
    "\"CpuUsage\":%d,"
    "\"MemUsage\":%d,"
    "\"WanUsage\":%d,"
    "\"LanUsage\":%d,"
    "\"IoUsage\":%d,"
    "\"LanIpState\":%d,"
    "\"WanIpState\":%d"
    "}",
    nrs->EpochTime,
    nrs->NodeId,
    nrs->CurrentConn,     
    nrs->CurrentBandwidth,
    nrs->DiskTotalSpace,
    nrs->DiskFreeSpace,
    (int)nrs->CpuUsage,
    (int)nrs->MemUsage,
    (int)nrs->WanUsage,
    (int)nrs->LanUsage,
    (int)nrs->IoUsage,
    nrs->LanIpState,
    nrs->WanIpState
  );

if(standalone) {
  return;
}

  if(sockfd == -1) {
    sockfd = createHttp(ip,port,SOCK_STREAM);
  }

  sendHttp(sockfd, url, connection, content);

/*analyze http content received
{"Status":1,"StatusDesc":"success"}

{"Status":0,"StatusDesc":"CheckFailed"}
*/
  memset(content, 0, sizeof(content));
  recvHttp(sockfd,content);
  //printf("ReportNodeStatusList() http content received:\n%s\n",content);

  ret = sscanf(content,
    "{"
    "\"Status\":%d,"
    "\"StatusDesc\":\"%[^\"]\""
    "}",
    &nrs->Status,
    nrs->StatusDesc
  );

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

  if(!strcasecmp(connection, "Close")){
    closeHttp(sockfd);
    sockfd = -1;
  }

if(!standalone) {
if(debugl >= 2) {
    if(nrs->Status == FAIL) {
      fprintf(stderr,"ReportNodeStatus() received FAIL: %s\n", nrs->StatusDesc);
    //  exit(1);
    }
}
}
}

