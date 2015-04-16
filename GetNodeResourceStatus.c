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
#include "GetNodeResourceStatus.h"

int cpu_average_interval=DEFAULT_CPU_AVERAGE_INTERVAL;  /*time interval to average */
int mem_average_interval=DEFAULT_MEM_AVERAGE_INTERVAL;  /*time interval to average */
int dsk_average_interval=DEFAULT_DSK_AVERAGE_INTERVAL;  /*time interval to average */
int net_average_interval=DEFAULT_NET_AVERAGE_INTERVAL;  /*time interval to average */
int con_average_interval=DEFAULT_CON_AVERAGE_INTERVAL;  /*time interval to average */
__attribute__((weak)) int debugl = DEFAULT_DEBUGL;

void GetNetworkConcernedStatus(struct net_data *data, char * ip, short port, float * usage, int * ipstate, long long * bandwidth) {
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
    fprintf(stderr,"ERROR: can't find ip [%s] from getifaddrs()\n",ip);
    exit(1);
  }

}

void GetFsDiskConcernedState(struct dsk_data * data, char *HomeDir, long long *DiskTotalSpace, long long *DiskFreeSpace, float* IoUsage)
{
  int i=0;
  char dir[HOMEDIR_LEN]={0};
  char *p=NULL;
  int root_reached=0;

  strcpy(dir, HomeDir);

  if((access(dir, 0)) == -1) {
    i = data->jfses;
if (debugl >= 2) {
    fprintf(stderr,"WARNING: HomeDir %s is not existent\n",HomeDir);
}
  }
  else {
    while(*dir) {
      for(i = 0; i<data->jfses; i++) {
        if(!strcmp(dir, data->jfs[i].name)) {
          goto SEARCH_HOMEDIR_END;
        }
      }

      if((p = strrchr(dir,'/')) != NULL) {
        *p = 0;
      }
      else {
        break;
      }

      if(!(*dir) && !root_reached) {
        root_reached=1;
        strcpy(dir,"/");
      }
    }
  }

SEARCH_HOMEDIR_END:
  if(i >= data->jfses) {
    *DiskTotalSpace = 0;
    *DiskFreeSpace  = 0;
    *IoUsage        = 0;
if (debugl >= 2) {
    fprintf(stderr,"WARNING: can't find directory %s from fstatfs()\n",HomeDir);
}
  }
  else {
    *DiskTotalSpace = data->jfs[i].size*1024*1024;
    *DiskFreeSpace  = data->jfs[i].free*1024*1024;
    *IoUsage        = data->jfs[i].io;
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

void GetNodeResourceStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs)
{
  static struct proc proc[P_NUMBER];
  static struct net_data net_data;
  static struct dsk_data dsk_data;
  static struct cpu_data cpu_data;
  static struct mem_data mem_data;
  static double con_time;
  long long cons = -1;

if(debugl >= 3) {
  printf("====================================================================================\n");
  printf("===========================   ReportNodeStatus() START   ===========================\n");
  printf("====================================================================================\n");
}
  //memset(nrs,0,sizeof(struct NodeResourceStatus));

  if(!proc[P_CPUINFO].filename) {
    proc_init(proc);
  }

  if(!cpu_data.p){
      cpu_data.p = &cpu_data.cpu_stats[0];
  }
  if(!cpu_data.q){
      cpu_data.q = &cpu_data.cpu_stats[1];
  }

  if(!dsk_data.p) {
      dsk_data.p = &dsk_data.dk[0];
  }
  if(!dsk_data.q) {
      dsk_data.q = &dsk_data.dk[1];
  }

  if(!mem_data.p) {
      mem_data.p = &mem_data.mem_stats[0];
  }
  if(!mem_data.q) {
      mem_data.q = &mem_data.mem_stats[1];
  }

  if(!net_data.p) {
      net_data.p = &net_data.net_stats[0];
  }
  if(!net_data.q) {
      net_data.q = &net_data.net_stats[1];
  }

  //Net
  if((doubletime() - net_data.p->time) >= net_average_interval) {
    GetNetworkState(&net_data);
    GetNetworkConcernedStatus(&net_data, nsl->WanIp, nsl->WanPort, &nrs->WanUsage, &nrs->WanIpState, &nrs->CurrentBandwidth);
    GetNetworkConcernedStatus(&net_data, nsl->LanIp, nsl->LanPort, &nrs->LanUsage, &nrs->LanIpState, NULL);
  }

  if((doubletime() - con_time) >= con_average_interval) {
    con_time = doubletime();
    cons = GetCurrentConn(nsl->WanIp, nsl->WanPort);
    if(cons != -1) nrs->CurrentConn = cons;
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
}

