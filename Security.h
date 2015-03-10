#ifndef SECURITY_H
#define SECURITY_H

#ifdef __cplusplus
extern "C"
{
#endif

#define  LEN_OF_KEY  24

extern int debugl;

extern int ContentEncode(char* enckey, char* encIv, char *input, char **output, int length);
                                                                         
extern int ContentDecode(char* deckey, char* decIv, char* input, char **output, int length);

#ifdef __cplusplus
}
#endif

#endif
