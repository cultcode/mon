#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "OperateXml.h"
#include "common.h"
#include "main.h"
#include "GetNodeResourceStatus.h"
#include "SocketHttp.h"

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
    "-a  --standalone   :run in standalone mode\n"
    "-b  --debugl       :debug info level, default to 1\n"
    "-r  --refresh      :interval of reporting status, unit is second. default to 1\n"
    "-i  --init         :specify URL for NodeStatusInit, like http://XXXX/ndas/NodeStatusInit\n"
    "-g  --get          :specify URL for GetNodeStatusList, like http://XXXX/ndas/GetNodeStatusList\n"
    "-p  --report       :specify URL for NodeStatusReport, like http://XXXX/ndas/NodeStatusReport\n"
    "-c  --cpu          :specify period for average cpu usage, unit is second\n"
    "-m  --mem          :specify period for average mem usage, unit is second\n"
    "-d  --dsk          :specify period for average dsk usage, unit is second\n"
    "-n  --net          :specify period for average net usage, unit is second\n"
    "-w  --wanip        :when in standalone mode, specify local ip:port\n"
    "-l  --lanip        :when in standalone mode, specify wide  ip:port\n"
    "-o  --homedir      :when in standalone mode, specify home direcotry\n"
    "-z  --zone         :specify timezone if necessary\n"
    "-t  --looptimes    :specify report times if necessary, default to dead-while\n"
    "-s  --svrversion   :Whether to insert Service Version into Init\n"
    "-q  --svrtype      :Service Type used in Init\n"
    "-u  --paramlisturl :specify URL for GetNodeSvrSysParamList\n"
    "-U  --paramlistiv  :specify interval for getting GetNodeSvrSysParamList\n"
    "-e  --waytogetcons :how to get connections, 0(default) is http, non-0 is from /proc\n"
    "-f  --logfile      :specify stdout&stderr redirection log file\n"
    "-j  --des_iv       :specify 3des iv\n"
    "-k  --des_key      :specify 3des key\n"
    "-h  --help         :print this help info\n"
    "-v  --version      :print version info\n"
    );
}

int ParseOptions(int argc,char**argv)
{
  int  i=0;
  int flags=0;
  char* const short_options = "a:b:r:i:g:p:c:m:d:n:C:w:l:o:z:t:s:q:u:U:e:f:hvj:k:";
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
    { "con",  1,  NULL,  'C'},  
    { "wanip",  1,  NULL,  'w'},  
    { "lanip",  1,  NULL,  'l'},  
    { "homedir",  1,  NULL,  'o'},  
    { "zone",  1,  NULL,  'z'},  
    { "looptimes",  1,  NULL,  't'},  
    { "svrversion",  1,  NULL,  's'},  
    { "svrtype",  1,  NULL,  'q'},  
    { "paramlisturl",  1,  NULL,  'u'},  
    { "paramlistiv",  1,  NULL,  'U'},  
    { "waytogetcons",  1,  NULL,  'e'},  
    { "logfile",  1,  NULL,  'f'},  
    { "des_key",  1,  NULL,  'k'},  
    { "des_iv",  1,  NULL,  'j'},  
    { "help",  0,  NULL,  'h'},  
    { "version",  0,  NULL,  'v'},  
    {  0,  0,  0,  0},  
  };

/********************************************************
 * reset all args value before get them
 ********************************************************/
  refresh_interval=DEFAULT_REFRESH_INTERVAL;
  standalone = DEFAULT_STANDALONE ;
  looptimes  = DEFAULT_LOOPTIMES ;

  memset(HomeDir,0,sizeof(HomeDir));
  memset(LanIp,0,sizeof(LanIp));
  memset(WanIp,0,sizeof(WanIp));
  LanPort = 0;
  WanPort = 0;
  memset(url,0,sizeof(url));

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
    case 'u':
      strcpy(url[3], optarg);
      break;
    case 'U':
      paramlist_interval = atoi(optarg);
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
    case 'C':
      con_average_interval = atoi(optarg);
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
    case 'f':
      if(strcmp(file_stdout, optarg)) {
        strcpy(file_stdout, optarg);
        strcpy(file_stderr, optarg);
        ReopenLog(0);
      }
      break;
    case 't':
      looptimes  = atoi(optarg);
      break;
    case 'h':
      usage();
      exit(0);
      break;
    case 'v':
      printf("%s Version %s\n",SelfName, VERSION);
      exit(0);
      break;
    case 's':
      svrversion  = atoi(optarg);
      break;
    case 'q':
      svrtype  = atoi(optarg);
      break;
    case 'j':
      strcpy(node_3des_iv, optarg);
      break;
    case 'k':
      strcpy(node_3des_key, optarg);
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
  printf("debugl: %d\ninit url: %s\nget url: %s\nreport url: %s\nparamlist url: %s\nparamlist_interval: %d, refresh interval %d\ncpu_average_interval %d\nmem_average_interval %d\ndsk_average_interval %d\nnet_average_interval %d\ncon_average_interval %d\nwanip:%s, wanport %hd, lanip %s, lanport %hd, homedir %s\nserver time zone %d\nnumber of loops %d\nway to get cons %d\nlogfile %s\nsvrtype %d,svrversion %d\n",debugl, url[0], url[1], url[2], url[3], paramlist_interval, refresh_interval, cpu_average_interval, mem_average_interval, dsk_average_interval, net_average_interval,con_average_interval,WanIp,WanPort,LanIp,LanPort,HomeDir,servertimezone, looptimes, waytogetcons,file_stdout,svrtype,svrversion);
}

return flags;

}

