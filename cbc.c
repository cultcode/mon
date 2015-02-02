#include <string.h>
#include <openssl/des.h>
#include "cbc.h"

char cbc_key[]="t^^BvGfAdUTixobQP$HhsOsD";
char cbc2_key[]="t^^BvGfAdUTixobQP$HhsOsD";
char cbc3_key[]="t^^BvGfAdUTixobQP$HhsOsD";
char cbc_iv[] = "=V#s%CS)";

unsigned char pkcs7[8][9]={
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00},
    {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00},
    {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x00, 0x00},
    {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00},
};

void cbc_decode(char *text, char *out)
{
  int n=1024;                               
  int i,j,k;   
  //int err;
  int len = strlen(text);

  unsigned char *input = (unsigned char *)text; 
  unsigned char *output = (unsigned char *)out; 

  DES_key_schedule key_schedule1, key_schedule2, key_schedule3;
  DES_cblock ivec;
  
  /*if((err = DES_set_key_unchecked((const_DES_cblock*)&cbc_key, &key_schedule1)) != 0)
    printf("Mysql proxy Key error %d\n",err);

  if((err = DES_set_key_unchecked((const_DES_cblock*)&cbc2_key, &key_schedule2)) != 0)
    printf("Mysql proxy Key error %d\n",err);

  if((err = DES_set_key_unchecked((const_DES_cblock*)&cbc3_key, &key_schedule3)) != 0)
    printf("Mysql proxy Key error %d\n",err);*/

  DES_set_key_unchecked((const_DES_cblock*)&cbc_key, &key_schedule1);
  DES_set_key_unchecked((const_DES_cblock*)&cbc2_key, &key_schedule2);
  DES_set_key_unchecked((const_DES_cblock*)&cbc3_key, &key_schedule3);
  memcpy(ivec, &cbc_iv, sizeof(cbc_iv));
  
  i=len/n;
  j=len%n;
    
  for (k=0;k<i;k++)
  {        
    DES_ede3_cbc_encrypt(&input[k*n],&output[k*n],n,
                   &key_schedule1,&key_schedule2,&key_schedule3,
                   &ivec,DES_DECRYPT);
  }

  DES_ede3_cbc_encrypt(&input[k*n],&output[k*n],j,
                 &key_schedule1,&key_schedule2,&key_schedule3,
                 &ivec,DES_DECRYPT);

  return;
}

int cbc_encode(char *text, char *out)
{
  int n=1024;                               
  int i,j,k, m;  
  int len, ret;
  unsigned char splice_pad[1024];
  //int err;

  unsigned char *input = (unsigned char *)text; 
  unsigned char *output = (unsigned char *)out; 

  DES_key_schedule key_schedule1, key_schedule2, key_schedule3;
  DES_cblock ivec;               
  
  /*if((err = DES_set_key_checked((const_DES_cblock*)&cbc_key, &key_schedule1)) != 0)
    printf("Mysql proxy Key error %d\n",err);

  if((err = DES_set_key_checked((const_DES_cblock*)&cbc2_key, &key_schedule2)) != 0)
    printf("Mysql proxy Key error %d\n",err);

  if((err = DES_set_key_checked((const_DES_cblock*)&cbc3_key, &key_schedule3)) != 0)
    printf("Mysql proxy Key error %d\n",err);*/

  DES_set_key_unchecked((const_DES_cblock*)&cbc_key, &key_schedule1);
  DES_set_key_unchecked((const_DES_cblock*)&cbc2_key, &key_schedule2);
  DES_set_key_unchecked((const_DES_cblock*)&cbc3_key, &key_schedule3);
  memcpy(ivec, &cbc_iv, sizeof(cbc_iv));
  
  len = strlen(text);
  i=len/n;
  j=len%n;

  ret = 0;
  for (k=0;k<i;k++)
  {        
    DES_ede3_cbc_encrypt(&input[k*n],&output[k*n],n,
                   &key_schedule1,&key_schedule2,&key_schedule3,
                   &ivec,DES_ENCRYPT);
    ret += n;
  }

  if(j)
    memcpy(splice_pad, &input[k*n], j);

  //if(j != 0)
  //{
    m = j%8;
    if(m)
    {
      sprintf((char *)&splice_pad[j], "%s", pkcs7[7-m]);
      j = (j+7)/8*8;
    }
    else
    {
      sprintf((char *)&splice_pad[j], "%s", pkcs7[7]);
      j = j + 8;
    }
    
    splice_pad[j] = '\0';
    
    DES_ede3_cbc_encrypt(splice_pad,&output[k*n],j,
                   &key_schedule1,&key_schedule2,&key_schedule3,
                   &ivec,DES_ENCRYPT);
    ret += j;
  //}

  return ret;
  
}

