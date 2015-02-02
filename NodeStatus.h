#ifndef NODESTATUS_H
#define NODESTATUS_H

#define CONNECTION_LEN 32

#define CONTENT_LEN 3072
#define CONTENTLEN_LEN 8

#define STATUSDESC_LEN 128
#define HOMEDIR_LEN 128
#define IP_LEN 32
#define URL_LEN 256
#define HOST_LEN 256
#define NODENAME_LEN 128
#define VERSION_LEN 32
#define VERSION "1.0"

enum {
  SUCESS=0,
#define SUCESS SUCESS
  FAIL=1
#define FAIL FAIL
};


#define STANDALONE

#endif
