#ifndef COMMON_H
#define COMMON_H

#define IP_LEN 32
#define PORT_LEN 32
#define URL_LEN 1024
#define FN_LEN 1024
#define HOMEDIR_LEN 128
#define VERSION_LEN 32

#define DEFAULT_SERVERTIMEZONE 8

extern int connect_timeout;
extern int transmit_timeout;
extern int connect_timeout_nms;
extern int transmit_timeout_nms;

extern char file_stdout[FN_LEN];
extern char file_stderr[FN_LEN];
extern int  getlist_interval;

extern int nStripNewLine(char *buf, int length);
extern void StripNewLine(char *buf);

extern long GetLocaltimeSeconds();

extern void InsertPort(char* url, char* url_o, short port, int rewrite);

extern void ParseUrl(char * url, char * protocol, char * host, short * port, char* path);

extern void ReopenLog(int signum);

extern int JoinChunk(char * chunked, char * seperator, char * content);
#endif
