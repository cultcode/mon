#include "OperateXml.h"

int ReadConfigXml(char * fn_xml, char ** opt)
{
  xmlDocPtr doc;
  xmlNodePtr curNode;
  xmlNodePtr propNodePtr ;
  xmlChar *szKey;
  xmlAttrPtr attrPtr;
  char *szDocName;

  doc = xmlReadFile(fn_xml,"UTF-8",XML_PARSE_RECOVER);


  if (NULL == doc) 
  {
    fprintf(stderr,"Document not parsed successfully\n");     
    return -1; 
  } 

  ;

  if ((curNode = xmlDocGetRootElement(doc)) == NULL)
  { 
    fprintf(stderr,"empty document\n"); 
    xmlFreeDoc(doc); 
    return -1; 
  } 

  if (xmlStrcmp(curNode->name, BAD_CAST "configuration")) 
  {
    fprintf(stderr,"document of the wrong type, root node %s != configuration\n",curNode->name); 
    xmlFreeDoc(doc); 
    return -1; 
  } 

  if ((curNode = curNode->xmlChildrenNode) == NULL)
  { 
    fprintf(stderr,"empty document\n"); 
    xmlFreeDoc(doc); 
    return -1; 
  } 


  if (xmlStrcmp(curNode->name, BAD_CAST "appSettings")) 
  {
    fprintf(stderr,"document of the wrong type, root node %s != appSettings\n",curNode->name); 
    xmlFreeDoc(doc); 
    return -1; 
  } 

  curNode = curNode->xmlChildrenNode
  propNodePtr = curNode;

  while(curNode != NULL) 
  {
     //取出节点中的内容
     if ((!xmlStrcmp(curNode->name, (const xmlChar *)"add"))) 
     {
         szKey = xmlNodeGetContent(curNode);
         printf("add: %s\n", szKey); 
         xmlFree(szKey); 
     } 
     //查找带有属性attribute的节点
     if (xmlHasProp(curNode,BAD_CAST "attribute"))
     {
         propNodePtr = curNode;
     }
     curNode = curNode->next; 

    //查找属性
    attrPtr = propNodePtr->properties;
    while (attrPtr != NULL)
    {
       if (!xmlStrcmp(attrPtr->name, BAD_CAST "attribute"))
       {
           xmlChar* szAttr = xmlGetProp(propNodePtr,BAD_CAST "attribute");
           cout           xmlFree(szAttr);
       }
       attrPtr = attrPtr->next;
    }
  } 

  xmlFreeDoc(doc);
  return 0;
}
