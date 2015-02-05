#ifndef COMMON_H
#define COMMON_H

extern int debugl;

//#define STANDALONE

extern long GetLocaltimeSeconds();

extern void ParseUrl(char * url, char * protocol, char * host, short * port, char* path);

#endif
