#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* strtol */
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "common.h"

char file_stdout[FN_LEN];
char file_stderr[FN_LEN];

void ReopenLog(int signum)
{
  FILE *fp=NULL;

  printf("Caught signal %d\n",signum);

  if((fp=fopen(file_stdout,"w")) == NULL) {
    perror("fopen");
    fprintf(stderr,"ERROR: %s\n",file_stdout);
  }
  else
  {
    fclose(fp);
    fclose(stdout);
    if((fp=freopen(file_stdout,"w", stdout)) == NULL) {
      perror("freopen stdout");
    }
  }

  if(!strcmp(file_stdout, file_stderr)) {
    dup2(fileno(stdout), fileno(stderr));
  }
  else {
    if((fp=fopen(file_stderr,"w")) == NULL) {
      perror("fopen");
      fprintf(stderr,"ERROR: %s\n",file_stderr);
    }
    else
    {
      fclose(fp);
      fclose(stderr);
      if((fp=freopen(file_stderr,"w", stderr)) == NULL) {
        perror("freopen stderr");
      }
    }
  }

}

int JoinChunk(char * chunked, char * seperator, char * content) {
  int state=0;
  char *p=chunked, *q=chunked;
  int length = 0;

  while (*p) {
    q = p;
    switch(state){
      case 0:   //length

        p=strstr(q, seperator);

        if(!p) {
          fprintf(stderr,"ERROR: strstr failed, string is\n%s\n",q);
          exit(1);
        }

        p+= strlen(seperator);

        state = 1;
        break;
      case 1:   //body

        p=strstr(q, seperator);

        if(!p) {
          fprintf(stderr,"ERROR: strstr failed, string is\n%s\n",q);
          exit(1);
        }

        length += p-q;
        strncat(content, q, p-q);

        p+= strlen(seperator);

        state = 0;
        break;
      default:
        fprintf(stderr,"ERROR: Illegal state in JoinChunk(): %d\n",state);
        exit(1);
        break;
    }
  }

  content[length] = 0;

  return length;
}

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

  if(!url) return;

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
        fprintf(stderr,"ERROR: illegal state %d\n",state);
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
