#ifndef GETNODERESOURCESTATUSLIST_H
#define GETNODERESOURCESTATUSLIST_H

#include "NodeResourceStatus.h"
#include "GetNodeStatusList.h"

extern int cpu_average_interval;  /*time interval to average */
extern int mem_average_interval;  /*time interval to average */
extern int dsk_average_interval;  /*time interval to average */
extern int net_average_interval;  /*time interval to average */

struct NodeResourceStatus{
  long  EpochTime;
  int   NodeId;
  long long CurrentConn;
  long long CurrentBandwidth;
  long long DiskTotalSpace;
  long long DiskFreeSpace;
  float CpuUsage;
  float MemUsage;
  float WanUsage;
  float LanUsage;
  float IoUsage;
  int   LanIpState;/*1:success 0:failure*/
  int   WanIpState;/*1:success 0:failure*/
  int   Status;/*1:success 0:failure*/
  char  StatusDesc[STATUSDESC_LEN];

};

extern void GetNodeResourceStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs);
#endif
