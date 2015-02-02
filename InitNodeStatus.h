#ifndef INITNODESTATUS_H
#define INITNODESTATUS_H

#include "NodeStatus.h"

struct NodeStatus {
  long  EpochTime;
  char  NodeName[NODENAME_LEN];
  char  Version[VERSION_LEN];
  int   Status;/*1:success 0:failure*/
  char  StatusDesc[STATUSDESC_LEN];
  int   NodeId;
};

extern void InitNodeStatus(struct NodeStatus* ns, char* url);

#endif
