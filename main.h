#ifndef MAIN_H
#define MAIN_H

#define DEFAULT_DEBUGL 1
#define DEFAULT_SERVERTIMEZONE 8
#define HOMEDIR_LEN 128
#define VERSION_LEN 32
#define VERSION "1.0"

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
extern char   url[3][URL_LEN];
extern char* SelfName;

#endif
