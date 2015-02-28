#ifndef SECURITY_H
#define SECURITY_H

#define  LEN_OF_KEY  24

#define NODE_3DES_KEY          node_3des_key

#define NODE_3DES_IV           node_3des_iv

//#define NODE_3DES_KEY          "t^^BvGfAdUTixobQP$HhsOsD"

//#define NODE_3DES_IV           "=V#s%CS)"

extern int ContentEncode(char* enckey, char* encIv, char *input, char **output, int length);
                                                                         
extern int ContentDecode(char* deckey, char* decIv, char* input, char **output, int length);

#endif
