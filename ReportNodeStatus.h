#ifndef REPORTNODESTATUSLIST_H
#define REPORTNODESTATUSLIST_H

#include "GetNodeResourceStatus.h"
extern int report_type;

extern void ReportNodeStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs, char * url);
#endif

