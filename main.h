#ifndef MAIN_H
#define MAIN_H

#define DEFAULT_REFRESH_INTERVAL 1
#define DEFAULT_STANDALONE 1
#define DEFAULT_LOOPTIMES -1

#include "common.h"

extern int refresh_interval; /*time interval to report data*/
extern int debugl;
extern int servertimezone;
extern int standalone;
extern int looptimes;

extern char   HomeDir[HOMEDIR_LEN];
extern char   LanIp[IP_LEN];
extern char   WanIp[IP_LEN];
extern short  LanPort;
extern short  WanPort;
extern char   url[4][URL_LEN];
extern char* SelfName;

extern int paramlist_interval;

extern int dsk_average_interval;

#endif
