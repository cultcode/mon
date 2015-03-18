#ifndef GETNODESVRSYSPARAMLIST_H
#define GETNODESVRSYSPARAMLIST_H

#include "NodeResourceStatus.h"

extern int dsk_average_interval;  /*time interval to average */

struct NodeSvrSysParamList {
  long  EpochTime;
  int   Status;/*1:success 0:failure*/
  char  StatusDesc[STATUSDESC_LEN];
  int   NS_ResMon_CollectRateDiskIO;
  int   NS_ResMon_ReportType;
};

extern void GetNodeSvrSysParamList(struct NodeSvrSysParamList* nsspl, char* url);

#endif
