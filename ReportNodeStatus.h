#ifndef REPORTNODESTATUSLIST_H
#define REPORTNODESTATUSLIST_H

#include "GetNodeResourceStatus.h"

#define REPORT_TYPE_LEN 8

extern char report_type_s[REPORT_TYPE_LEN];
extern int port_udp;

extern void ReportNodeStatus(struct NodeStatusList* nsl, struct NodeResourceStatus* nrs, char * url);
#endif

