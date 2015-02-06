#ifndef NODESTATUS_H
#define NODESTATUS_H

#define CONNECTION_LEN 32

#define CONTENT_LEN 3072
#define CONTENTLEN_LEN 8

#define STATUSDESC_LEN 128
#define HOMEDIR_LEN 128

#define PROTOCOL_LEN 16
#define IP_LEN 32
#define PORT_LEN 32
#define HOST_LEN 64
#define PATH_LEN 256

#define FILE_LINE_BUFFER 1024
#define FN_LEN 1024
#define NODENAME_LEN 128
#define VERSION_LEN 32
#define VERSION "1.0"

enum {
  FAIL=0,
  SUCESS=1
};

#include "common.h"

#define SERVER_VERSION 1

extern int cpu_average_interval;  /*time interval to average */
extern int mem_average_interval;  /*time interval to average */
extern int dsk_average_interval;  /*time interval to average */
extern int net_average_interval;  /*time interval to average */

#endif
