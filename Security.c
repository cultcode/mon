#include <string.h>
#include <stdio.h>
#include <math.h>
#include <openssl/des.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "common.h"
#include "main.h"
#include "Security.h"

static int DesCbcEncode(char* enckey, char* encIv, char *input,  char **output, int length)
{
  DES_key_schedule ks1, ks2, ks3;
  DES_cblock ivec={0};
  unsigned char ch=0,use_key[LEN_OF_KEY+1]={0}, block_key[9]={0};
  unsigned char *src=NULL,*dst=NULL;
  unsigned int data_rst=0,encdata_len=0,txtdata_len=0;
  size_t i=0,count=0;
       
if (debugl >= 3) {
  printf("3des cbc encode, original length is %d\n",length);
}

  //deal key
  if(strlen(enckey) >= LEN_OF_KEY)
  {
    memcpy(use_key,enckey,LEN_OF_KEY);
    use_key[LEN_OF_KEY] = 0;
  }
  else
  {
    memcpy(use_key,enckey,strlen(enckey));
    memset(use_key+strlen(enckey),0x00,LEN_OF_KEY-strlen(enckey));
  }
  
  //deal source data
  txtdata_len = length;
  data_rst = txtdata_len % 8;
  ch = 8 - data_rst;

  encdata_len= txtdata_len + (8-data_rst);

  if((src = (unsigned char *)calloc(encdata_len+1, sizeof(char))) == NULL) {
    perror("calloc() failed");
    return -1;
  }

  //because 3des is symmetric
  if((*output = (char *)calloc(encdata_len+1, sizeof(char))) == NULL) {
    perror("calloc() failed");
    return -1;
  }

  dst = (unsigned char *)(*output);
        
  memcpy(src,input,txtdata_len);
  
  memset(src+txtdata_len,ch,ch);

  memset(block_key,0,sizeof(block_key));
  memcpy(block_key,use_key,8);
  DES_set_key_unchecked((const_DES_cblock*)&block_key, &ks1);

  memset(block_key,0,sizeof(block_key));
  memcpy(block_key,use_key + 8,8);
  DES_set_key_unchecked((const_DES_cblock*)&block_key, &ks2);

  memset(block_key,0,sizeof(block_key));
  memcpy(block_key,use_key + 16,8);
  DES_set_key_unchecked((const_DES_cblock*)&block_key, &ks3);

  if(strlen(encIv) != sizeof(ivec)) {
    fprintf(stderr,"length of encIv (%zu) is not equal to sizeof(DES_cblock) (%lu)\n",strlen(encIv), sizeof(ivec));
    return -1;
  }

  memcpy(ivec,encIv,strlen(encIv));
  
  count = encdata_len / 8;

  for(i = 0;i < count; i++)
  {
    DES_ede3_cbc_encrypt(&src[8*i],&dst[8*i],8,&ks1,&ks2,&ks3,&ivec,DES_ENCRYPT);
  }

//  dst[8*i] = 0;

  free(src);
  src = NULL;     

  return encdata_len;
}

static int DesCbcDecode(char * deckey,char * decIv,char * input, char **output, int length)
{
  DES_key_schedule ks1, ks2, ks3;
  DES_cblock ivec={0};
  unsigned char ch,use_key[LEN_OF_KEY+1]={0}, block_key[9]={0};
  unsigned char *src=NULL,*dst=NULL;
  unsigned int data_rst=0,decdata_len=0;
 
  size_t i=0,count=0;

if (debugl >= 3) {
  printf("3des cbc decode, original length is %d\n",length);
}

  //deal key
  if(strlen(deckey) >= LEN_OF_KEY)
  {
     memcpy(use_key,deckey,LEN_OF_KEY);
     use_key[LEN_OF_KEY] = 0;
  }
  else
  {
     memcpy(use_key,deckey,strlen(deckey));
     memset(use_key+strlen(deckey),0x00,LEN_OF_KEY-strlen(deckey));
  }

  data_rst = length % 8;
  ch = 8 - data_rst;

  if(data_rst > 0)
  {
    decdata_len = length + (8-data_rst);
  }
  else
  {
    decdata_len = length;
  }

  src = (unsigned char *)input;

  //because 3des is symmetric
  if((*output = (char *)calloc(decdata_len+1, sizeof(char))) == NULL) {
    perror("calloc() failed");
    return -1;
  }

  dst = (unsigned char *)(*output);

  if(src != NULL)  
  {        
    memset(block_key,0,sizeof(block_key));

    memset(block_key,0,sizeof(block_key));
    memcpy(block_key,use_key,8);      
    DES_set_key_unchecked((const_DES_cblock*)&block_key, &ks1);

    memset(block_key,0,sizeof(block_key));
    memcpy(block_key,use_key + 8,8);
    DES_set_key_unchecked((const_DES_cblock*)&block_key, &ks2);     
 
    memset(block_key,0,sizeof(block_key));
    memcpy(block_key,use_key + 16,8);      
    DES_set_key_unchecked((const_DES_cblock*)&block_key, &ks3);
 
    count = decdata_len / 8;
    memcpy(ivec,decIv,strlen(decIv));

    for(i = 0;i < count; i++)
    {
      DES_ede3_cbc_encrypt(&src[8*i],&dst[8*i],8,&ks1,&ks2,&ks3,&ivec,DES_DECRYPT);
    }
    ch = dst[8*i-1];

    if(ch < 9) {
      dst[8*i - ch] = 0;
    }     
  }

  return decdata_len;
}

static int Base64Encode(const char* message, char** buffer, int length) { //Encodes a string to base64
  BIO *bio, *b64;
  int ret=0;
  FILE* stream;

  int encodeSize = 4*ceil((double)length/3);

if (debugl >= 3) {
  printf("Base64Encode(), origian  is %d,processed length is %d \n",length,encodeSize);
}

  if((*buffer = malloc(encodeSize+1)) ==NULL) {
    perror("malloc() failed");
    return -1;
  }
  memset(*buffer,0,encodeSize+1);

  if((stream = fmemopen(*buffer, encodeSize+1, "w")) == NULL) {
    perror("fmemopen()");
    return -1;
  }

  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  b64 = BIO_new(BIO_f_base64());
  BIO_push(b64, bio);
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

  if((ret = BIO_write(b64, message, length)) != length) {
    fprintf(stderr,"BIO_write() return value is %d, while %d is expected\n",ret, length);
    return -1;
  }

  ret = BIO_flush(b64);
  BIO_free_all(b64);
  fclose(stream);

  return encodeSize;
}

static int calcDecodeLength(const char* b64input, int len) { //Calculates the length of a decoded base64 string
  int padding = 0;
 
  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;
 
  return (int)len*0.75 - padding;
}

static int Base64Decode(char* b64message, char** buffer, int length) { //Decodes a base64 encoded string
  BIO *bio, *b64;
  int ret=0;
  FILE* stream;

  int decodeSize = calcDecodeLength(b64message,length);

  if((*buffer = malloc(decodeSize+1)) ==NULL) {
    perror("malloc() failed");
    return -1;
  }
  memset(*buffer,0,decodeSize+1);

  if((stream = fmemopen(b64message, length, "r")) == NULL) {
    perror("fmemopen()");
    return -1;
  }

  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  b64 = BIO_new(BIO_f_base64());
  BIO_push(b64, bio);
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer

    //Can test here if len == decodeLen - if not, then return an error
  if((ret = BIO_read(b64, *buffer, length)) != decodeSize) {
    fprintf(stderr,"BIO_read() return value is %d, while %d is expected\n",ret, decodeSize);
    return -1;
  }

  BIO_free_all(b64);
  fclose(stream);

if (debugl >= 3) {
  printf("Base64Decode(), origian length is %d,processed length is %d\n",length ,decodeSize);
}

  return decodeSize; //success
}

int ContentEncode(char* enckey, char* encIv, char *input, char **output, int length)
{
  int length_after_des=0;
  int length_after_b64=0;
  char * middle=NULL;

if (debugl >= 3) {
  printf("ContentEncode(), origian length is %d string is \n%s\n",length ,input);
}

  length_after_des = DesCbcEncode(enckey,encIv,input,&middle,length);

  if(length_after_des<=0) {
    free(middle);
    return length_after_des;
  }

  length_after_b64 = Base64Encode(middle, output, length_after_des);

  free(middle);

  return length_after_b64;
}

int ContentDecode(char * deskey,char * desIv,char * input, char **output, int length)
{
  int length_after_b64=0;
  int length_after_des=0;
  char * middle=NULL;

if (debugl >= 3) {
  printf("ContentDecode(), origian length is %d string is \n%s\n",length ,input);
}

  length_after_b64 = Base64Decode(input, &middle, length);

  if(length_after_b64<=0) {
    free(middle);
    return length_after_b64;
  }

  length_after_des = DesCbcDecode(deskey,desIv,middle,output,length_after_b64);

  free(middle);

  return length_after_des;
}

