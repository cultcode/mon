#ifndef COMMON_H
#define COMMON_H

#define DEBUGL 1
//#define STANDALONE

extern long GetLocaltimeSeconds();

extern void ParseUrl(char * url, char * protocol, char * host, short * port, char* path);

#endif
