#ifndef MAIN_H
#define MAIN_H

#define DEFAULT_DEBUGL 1
#define HOMEDIR_LEN 128

#include "common.h"

extern int refresh_interval; /*time interval to report data*/
extern int cpu_average_interval;  /*time interval to average */
extern int mem_average_interval;  /*time interval to average */
extern int dsk_average_interval;  /*time interval to average */
extern int net_average_interval;  /*time interval to average */

extern int standalone;

extern int debugl;

extern int servertimezone;

extern int looptimes;

extern int servegoal;

char   url[3][URL_LEN];

extern char   HomeDir[HOMEDIR_LEN];
extern char   LanIp[IP_LEN];
extern char   WanIp[IP_LEN];
extern short  LanPort;
extern short  WanPort;

extern int waytogetcons;
extern char* SelfName;

#endif
