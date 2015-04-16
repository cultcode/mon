#ifndef REPORTNODESTATUSLIST_H
#define REPORTNODESTATUSLIST_H

#include "GetNodeResourceStatus.h"
extern char report_type_s[8];
extern int report_type;
extern int port_udp;

extern void ReportNodeStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs, char * url);
#endif

