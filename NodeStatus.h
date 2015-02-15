#ifndef NODESTATUS_H
#define NODESTATUS_H

#include "common.h"
#include "main.h"

#define CONNECTION_LEN 32

#define CONTENT_LEN 3072
#define CONTENTLEN_LEN 8

#define STATUSDESC_LEN 128

#define PROTOCOL_LEN 16
#define HOST_LEN 64
#define PATH_LEN 256

#define FILE_LINE_BUFFER 1024
#define FN_LEN 1024
#define NODENAME_LEN 128
#define VERSION_LEN 32
#define VERSION "1.0"
#define SLAVES_LEN 1024

#define SERVER_VERSION 1

enum {
  FAIL=0,
  SUCESS=1
};

#endif
