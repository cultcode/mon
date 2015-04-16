#ifndef COMMON_H
#define COMMON_H

#define IP_LEN 32
#define PORT_LEN 32
#define URL_LEN 1024
#define FN_LEN 1024
#define HOMEDIR_LEN 128
#define VERSION_LEN 32

#define DEFAULT_SERVERTIMEZONE 8

extern char file_stdout[FN_LEN];
extern char file_stderr[FN_LEN];

extern void StripNewLine(char *buf);

extern long GetLocaltimeSeconds();

extern void InsertPort(char* url, char* url_o, short port);

extern void ParseUrl(char * url, char * protocol, char * host, short * port, char* path);

extern void ReopenLog(int signum);

extern int JoinChunk(char * chunked, char * seperator, char * content);
#endif
