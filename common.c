#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* strtol */
#include <time.h>
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
  time_t t=time(NULL);

  t += servertimezone * 3600;
if (debugl >= 4) {
  printf("GetLocaltimeSeconds() %ld %#lx\n",t,t);
}

  return (long)t;
}

void ParseUrl(char * url, char * protocol, char * host, short * port, char* path) {
  int j=0, k=0;
  int state = 0;
  char field[URL_LEN]={0};

  *port = 0;

  //http://192.168.8.224:9000/ndas/NodeResMonServerInit
  while(url[j]) {
    switch(state) {
      case 0://protocol
        if((url[j] == ':') || (url[j] == '/')) {
          field[k] = 0;
          k=0;
          if(strchr(field, '.')) {  //is host/host
            if(host) {strcpy(host,field);}
            if(url[j] == '/') {
              state = 3;
              j--;
            }
            else {
              state = 2;
            }
          }
          else {  // is protocol
            if(protocol) {strcpy(protocol,field);}
            j+=2;
            state = 1;
          }
        }
        else {
          field[k++] = url[j];
        }
        break;
      case 1://host
        if((url[j] == ':') || (url[j] == '/')) {
          field[k] = 0;
          k = 0;
          if(host) {strcpy(host,field);}
          if(url[j] == '/') {
            state = 3;
            j--;
          }
          else {
            state = 2;
          }
        }
        else {
          field[k++] = url[j];
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

  if(state == 1) {
    field[k] = 0;
    if(host) {strcpy(host,field);}
  }

if (debugl >= 3) {
  printf("url:%s\nprotocol:%s\thost:%s\tport:%hd,path:%s\n",url, protocol, host, *port, path);
}

}
