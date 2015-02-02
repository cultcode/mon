#ifndef SOCKETHTTP_H
#define SOCKETHTTP_H

#define HTTP_LEN 4096

extern int createHttp(char * ip, short port);

extern void closeHttp(int sockfd);

extern void sendHttp(int sockfd,char * request_line, char * host, char * content_type, char * content_len, char * connection, char * content);

extern void recvHttp(int sockfd, char* str);

#endif
