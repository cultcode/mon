#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* strtol */
#include <sys/time.h>
#include "common.h"


void StripNewLine(char *buf)
{
  int i=0;
  //strip the character of line feed / carrier return
  for (i = (strlen(buf) -1); i>=0; i--) {
    if((buf[i] == '\n') || (buf[i] == '\r')) {
      buf[i] = 0;
    }
    else {
      break;
    }
  }

}

long GetLocaltimeSeconds()
{
  long t=0;
  struct timeval tv={0};
  struct timezone tz={0};

  gettimeofday(&tv,&tz);

  t += tv.tv_sec;
  t -= tz.tz_minuteswest*60;

if (debugl >= 4) {
  printf("GetLocaltimeSeconds() %ld %#lx\n",t,t);
}

  return t;
}

void ParseUrl(char * url, char * protocol, char * host, short * port, char* path) {
  int j=0, k=0;
  int state = 0;

  *port = 0;

  if(protocol == NULL) {
    state = 1;
  }

  //http://192.168.8.224:9000/ndas/NodeResMonServerInit
  while(url[j]) {
    switch(state) {
      case 0://protocol
        if(url[j] == ':') {
          j+=2;
          state = 1;
          if(protocol) {protocol[k] = 0;}
          k=0;

        }
        else {
          if(protocol) {protocol[k++] = url[j];}
        }
        break;
      case 1://host
        if(url[j] == ':') {
          state = 2;
          host[k] = 0;
          k = 0;
        }
        else if(url[j] == '/') {
          state = 3;
          j--;
          host[k] = 0;
          k = 0;
        }
        else {
          host[k++] = url[j];
        }
        break;
      case 2://port
        if(!*port) {
          *port = strtol(&url[j],NULL,0);
        }

        if(url[j] == '/') {
          state = 3;
          j--;
        }
        break;
      case 3://path
          if(path) {path[k++] = url[j];}
        break;
      default:
        fprintf(stderr,"illegal state %d\n",state);
        exit(1);
    }
    j++;
  }

if (debugl >= 3) {
  printf("url:%s\nprotocol:%s\tip:%s\tport:%hd,path:%s\n",url, protocol, host, *port, path);
}

}
