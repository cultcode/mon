#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* strtol */
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
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

long GetLocaltimeSeconds(int servertimezone)
{
  time_t t=time(NULL);

  t += servertimezone * 3600;
//if (debugl >= 4) {
//  printf("GetLocaltimeSeconds() %ld %#lx\n",t,t);
//}

  return (long)t;
}

void ParseUrl(char * url, char * protocol, char * ip, short * port, char* path) {
  int j=0, k=0;
  int state = 0;
  char field[URL_LEN]={0};
  char host[URL_LEN]={0};
  char port_s[PORT_LEN]={0};

  char str[IP_LEN]={0};
  char **pptr=NULL;
  struct hostent *hptr=NULL;

  *port = 0;

  //dbagent.cdn.imgo.tv:22
  //http://192.168.8.224:9000/ndas/NodeResMonServerInit
  while(url[j]) {
    switch(state) {
      case 0://protocol
        if((url[j] == ':') || (url[j] == '/')) {
          field[k] = 0;
          k=0;
          if(strchr(field, '.')) {  //is host/ip
            strcpy(host,field);
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
          strcpy(host,field);
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
        if(url[j] == '/') {
          field[k] = 0;
          k = 0;
          state = 3;
          j--;
          strcpy(port_s, field);
        }
        else {
          field[k++] = url[j];
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

  if(k){
    field[k] = 0;
    k=0;
  }

  switch(state) {
    case 0:
    case 1:
      strcpy(host,field);
      break;
    case 2:
      strcpy(port_s,field);
      break;
  } 

  if((host[0] >= '0') && (host[0] <= '9')) {
    if(ip) {strcpy(ip, host);}
  }
  else {
    if( (hptr = gethostbyname(host) ) == NULL )
    {
      herror("gethostbyname()");
      fprintf(stderr,"ERROR: host:%s\n",host);
      exit(1);
    }
    switch(hptr->h_addrtype)
    {
    case AF_INET:
    case AF_INET6:
      pptr=hptr->h_addr_list;
      for(;*pptr!=NULL;pptr++) {
        if(inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)) == NULL) {
          perror("inet_ntop()");
          exit(1);
        }
      }
      break;
    default:
      printf("unknown address type/n");
      break;
    }

    if(ip) {strcpy(ip, str);}
  }

  if(port) {
    if(strlen(port_s)) {
      *port = strtol(field,NULL,0);
    }
    else {
      *port = 80;
    }
  }
  else {
  }

//if (debugl >= 3) {
//  printf("url:%s\nprotocol:%s\thost:%s[ip:%s]\tport:%hd,path:%s\n",url, protocol, host,ip, *port, path);
//}

}
