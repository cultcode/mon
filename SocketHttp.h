#ifndef SOCKETHTTP_H
#define SOCKETHTTP_H

#define HTTP_HEADER_LEN 1024
#define HTTP_LEN 4096
#define HTTP_NEWLINE "\r\n"

extern int createHttp(char * ip, short port, int type);

extern void closeHttp(int sockfd);

extern void sendHttp(int sockfd, char * url, char * connection, char * input, int encode, char *extra_header);

extern void recvHttp(int sockfd, char* output, int encode);

#endif
