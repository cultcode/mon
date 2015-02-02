#ifndef REPORTNODESTATUSLIST_H
#define REPORTNODESTATUSLIST_H

#include "NodeStatus.h"
#include "GetNodeStatusList.h"

struct NodeResourceStatus{
  long  EpochTime;
  int   NodeId;
  long long CurrentConn;
  int CurrentBandwidth;
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

extern void ReportNodeStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs, char * url);
#endif
