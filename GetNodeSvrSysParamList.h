#ifndef GETNODESVRSYSPARAMLIST_H
#define GETNODESVRSYSPARAMLIST_H

#include "NodeResourceStatus.h"
#include "InitNodeStatus.h"
#include "ReportNodeStatus.h"
#include "SocketHttp.h"

extern int dsk_average_interval;  /*time interval to average */

struct NodeSvrSysParamList {
  long  EpochTime;
  int   NodeId;
  int   Status;/*1:success 0:failure*/
  char  StatusDesc[STATUSDESC_LEN];
  int   NS_ResMon_CollectRateDiskIO;
  int   NS_ResMon_CollectRateIP;
  int   NS_ResMon_CollectRateNetFlow;
  char  NS_ResMon_ReportType[REPORT_TYPE_LEN];
  char  des3_key[KEY_LEN];
  char  des3_iv[KEY_LEN];
};

extern void GetNodeSvrSysParamList(struct NodeStatus* ns, struct NodeSvrSysParamList* nsspl, char* url);

#endif
