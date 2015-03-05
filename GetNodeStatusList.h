#ifndef GETNODESTATUSLIST_H
#define GETNODESTATUSLIST_H

#include "NodeResourceStatus.h"
#include "InitNodeStatus.h"

struct NodeStatusList{
  long   EpochTime;
  int    NodeId;
  int    Status;/*1:success 0:failure*/
  char   StatusDesc[STATUSDESC_LEN];
  char   HomeDir[HOMEDIR_LEN];
  char   LanIp[IP_LEN];
  char   WanIp[IP_LEN];
  short  LanPort;
  short  WanPort;
};

extern void GetNodeStatusList(struct NodeStatus* ns, struct NodeStatusList* nsl, char * url);
#endif
