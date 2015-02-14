#include <string.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "OperateXml.h"

int ReadConfigXml(char * fn_xml, char ** opt)
{
  xmlDocPtr doc=NULL;
  xmlNodePtr curNode=NULL;
  xmlChar* szAttr=NULL;
  int count=0;

  if((*opt = malloc(1024)) == NULL) {
    perror("malloc");
    exit(1);
  }

  if ((doc = xmlReadFile(fn_xml,"UTF-8",XML_PARSE_RECOVER)) == NULL) 
  {
    fprintf(stderr,"Document %s not parsed successfully\n",fn_xml);     
    exit(1);
  } 

  if (((curNode = xmlDocGetRootElement(doc)) == NULL) || (xmlStrcmp(curNode->name, BAD_CAST "configuration")))
  { 
    fprintf(stderr,"wrong Node configuration\n"); 
    xmlFreeDoc(doc); 
    exit(1); 
  } 

  if (((curNode = curNode->children) == NULL) || (xmlStrcmp(curNode->name, BAD_CAST "appSettings")))
  { 
    fprintf(stderr,"wrong Node appSettings\n"); 
    xmlFreeDoc(doc); 
    exit(1); 
  } 

  curNode = curNode->children;

  while(curNode != NULL) 
  {
    if ((xmlStrcmp(curNode->name, (const xmlChar *)"add"))) 
    {
      fprintf(stderr,"node %s != add\n",curNode->name); 
      count = -1;
      break;
    } 

    if((szAttr = xmlGetProp(curNode,BAD_CAST "key")) == NULL) {
      fprintf(stderr,"No attribute key\n");
      count = -1;
      break;
    }
    strcat(*opt,(char*)szAttr);
    count++;
    xmlFree(szAttr);

    if((szAttr = xmlGetProp(curNode,BAD_CAST "value")) == NULL) {
      fprintf(stderr,"No attribute value\n");
      count = -1;
      break;
    }
    strcat(*opt,(char*)szAttr);
    count++;
    xmlFree(szAttr);

    curNode = curNode->next;
  }

  xmlFreeDoc(doc);
  return count;
}
