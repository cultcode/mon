#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "OperateXml.h"
#include "common.h"
#include "main.h"

int ReadConfigXml(char * fn_xml, char *** opt)
{
  xmlDocPtr doc=NULL;
  xmlNodePtr curNode=NULL;
  xmlChar* szAttr=NULL;
  int count=0;

  if(((*opt) = (char**)malloc(OPTIONS_LEN*sizeof(char*))) == NULL) {
    perror("malloc");
    exit(1);
  }
  memset((*opt), 0, OPTIONS_LEN*sizeof(char*));


  if(((*opt)[count] = malloc(OPTION_LEN)) == NULL) {
    perror("malloc");
    exit(1);
  }
  memset((*opt)[count], 0, OPTION_LEN);
  strcpy((*opt)[count],SelfName);
  count++;

  //read file
  if ((doc = xmlReadFile(fn_xml,NULL,XML_PARSE_RECOVER)) == NULL) 
  {
    printf("Document %s not parsed successfully\n",fn_xml);     
    return count;
  } 
  else {
    printf("Be Noticed: reading configure arguments from %s......\n",fn_xml);
  }

  //get root node, e.g. <configuration/> and check error
  if (((curNode = xmlDocGetRootElement(doc)) == NULL) || (xmlStrcmp(curNode->name, BAD_CAST "configuration")))
  { 
    printf("wrong Node configuration\n"); 
    xmlFreeDoc(doc); 
    return count;
  } 

  //get <appSettings/> and check error
  curNode = curNode->children;
  while(curNode)
  {
    if((curNode->type == XML_ELEMENT_NODE) && (!xmlStrcmp(curNode->name, BAD_CAST "appSettings")))
    {
      break;
    }

    curNode = curNode->next;
  }

  if(!curNode)
  {
    printf("wrong Node appSettings\n"); 
    xmlFreeDoc(doc); 
    return count;
  }

  //get <add\> and check error
  curNode = curNode->children;
  for(;curNode;curNode = curNode->next)
  {
    if((curNode->type == XML_ELEMENT_NODE) && (!xmlStrcmp(curNode->name, BAD_CAST "add")))
    {
      if((szAttr = xmlGetProp(curNode,BAD_CAST "key")) == NULL) {
        printf("No attribute key\n");
        //count = -1;
        break;
      }

      if(((*opt)[count] = malloc(OPTION_LEN)) == NULL) {
        perror("malloc");
        exit(1);
      }
      memset((*opt)[count], 0, OPTION_LEN);

      if(strlen((char*)szAttr) == 1) {
        strcat((*opt)[count], "-");
      }
      else {
        strcat((*opt)[count], "--");
      }

      strcat((*opt)[count],(char*)szAttr);

      count++;
      xmlFree(szAttr);

      if((szAttr = xmlGetProp(curNode,BAD_CAST "value")) == NULL) {
        printf("No attribute value\n");
        //count = -1;
        break;
      }

      if(((*opt)[count] = malloc(OPTION_LEN)) == NULL) {
        perror("malloc");
        exit(1);
      }
      memset((*opt)[count], 0, OPTION_LEN);
      strcat((*opt)[count],(char*)szAttr);

      count++;
      xmlFree(szAttr);
    } 

  }

  xmlFreeDoc(doc);
  xmlCleanupParser();

  return count;
}

void usage() {
  printf(
    "-a  --standalone :run in standalone mode\n"
    "-b  --debugl     :debug info level, default to 1\n"
    "-r  --refresh    :interval of reporting status, unit is second. default to 1\n"
    "-i  --init       :specify URL for NodeStatusInit, like http://XXXX/ndas/NodeStatusInit\n"
    "-g  --get        :specify URL for GetNodeStatusList, like http://XXXX/ndas/GetNodeStatusList\n"
    "-p  --report     :specify URL for NodeStatusReport, like http://XXXX/ndas/NodeStatusReport\n"
    "-c  --cpu        :specify period for average cpu usage, unit is second\n"
    "-m  --mem        :specify period for average mem usage, unit is second\n"
    "-d  --dsk        :specify period for average dsk usage, unit is second\n"
    "-n  --net        :specify period for average net usage, unit is second\n"
    "-w  --wanip      :when in standalone mode, specify local ip:port\n"
    "-l  --lanip      :when in standalone mode, specify wide  ip:port\n"
    "-h  --homedir    :when in standalone mode, specify home direcotry\n"
    "-z  --zone       :specify timezone if necessary\n"
    "-t  --looptimes  :specify report times if necessary, default to dead-while\n"
    "-s  --servegoal  :specify server program type, 2 is for 2nd CDN server, 3 is for 3rd CDN server, default to 3\n"
    "-e  --connections:how to get connections, 0(default) is http, non-0 is from /proc\n"
    "-h  --help       :print this help info\n"
    );
}

int ParseOptions(int argc,char**argv)
{
  int  i=0;
  int flags=0;
  char* const short_options = "a:b:r:i:g:p:c:m:d:n:w:l:o:z:t:s:e:h";
  struct option long_options[] = {  
    { "standalone",  1,  NULL,  'a'},  
    { "debugl",  1,  NULL,  'b'},  
    { "refresh",  1,  NULL,  'r'},  
    { "init",  1,  NULL,  'i'},  
    { "get",  1,  NULL,  'g'},  
    { "report",  1,  NULL,  'p'},  
    { "cpu",  1,  NULL,  'c'},  
    { "mem",  1,  NULL,  'm'},  
    { "dsk", 1,  NULL,  'd'},  
    { "net",  1,  NULL,  'n'},  
    { "wanip",  1,  NULL,  'w'},  
    { "lanip",  1,  NULL,  'l'},  
    { "homedir",  1,  NULL,  'o'},  
    { "zone",  1,  NULL,  'z'},  
    { "looptimes",  1,  NULL,  't'},  
    { "servegoal",  1,  NULL,  's'},  
    { "waytogetcons",  1,  NULL,  'e'},  
    { "help",  0,  NULL,  'h'},  
    {  0,  0,  0,  0},  
  };

if(debugl >= 1) {
  printf("number of arguments: %d\narguments: ",argc);
  for(i=0;i<argc;i++) {
    printf("%s ",argv[i]);
  }
  printf("\n");
}

/********************************************************
 * get input variables
 ********************************************************/
  optind = 1;
  while ( -1 != (i = getopt_long(argc, argv, short_options, long_options, NULL))) {
    switch (i) {
    case 'a':
      standalone = atoi(optarg);
      break;
    case 'b':
      debugl = atoi(optarg);
      break;
    case 'r':
      refresh_interval = atoi(optarg);
      break;
    case 'i':
      strcpy(url[0], optarg);
      break;
    case 'g':
      strcpy(url[1], optarg);
      break;
    case 'p':
      strcpy(url[2], optarg);
      break;
    case 'c':
      cpu_average_interval = atoi(optarg);
      break;
    case 'm':
      mem_average_interval = atoi(optarg);
      break;
    case 'd':
      dsk_average_interval = atoi(optarg);
      break;
    case 'n':
      net_average_interval = atoi(optarg);
      break;
    case 'w':
      //standalone  = 1;
      ParseUrl(optarg,NULL,WanIp,&WanPort,NULL);
      break;
    case 'l':
      //standalone  = 1;
      ParseUrl(optarg,NULL,LanIp,&LanPort,NULL);
      break;
    case 'o':
      //standalone  = 1;
      strcpy(HomeDir, optarg);
      break;
    case 'z':
      servertimezone  = atoi(optarg);
      break;
    case 'e':
      waytogetcons = atoi(optarg);
      break;
    case 't':
      looptimes  = atoi(optarg);
      break;
    case 'h':
      usage();
      exit(0);
      break;
    case 's':
      servegoal  = atoi(optarg);
      break;
    default:
      flags=1;
      break;
    }
  }

  if(standalone & (debugl==DEFAULT_DEBUGL)) {
    debugl = 3;
  }

if (debugl >= 1) {
  printf("debugl: %d\ninit url: %s\nget url: %s\nreport url: %s\nrefresh interval %d\ncpu_average_interval %d\nmem_average_interval %d\ndsk_average_interval %d\nnet_average_interval %d\nwanip:%s, wanport %hd, lanip %s, lanport %hd, homedir %s\nserver time zone %d\nnumber of loops %d\nway to get cons %d\n",debugl, url[0], url[1], url[2], refresh_interval, cpu_average_interval, mem_average_interval, dsk_average_interval, net_average_interval,WanIp,WanPort,LanIp,LanPort,HomeDir,servertimezone, looptimes, waytogetcons);
}

return flags;

}

