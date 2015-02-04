//Encodes Base64
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "Base64Convert.h"
 
int Base64Encode(const char* message, char* buffer, int length) { //Encodes a string to base64
  BIO *bio, *b64;
  int length_p = 0, ret=0;
  FILE* stream;

  int encodedSize = 4*ceil((double)length/3);

  if((stream = fmemopen(buffer, encodedSize, "w")) == NULL) {
    perror("fmemopen()");
    exit(1);
  }

  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  b64 = BIO_new(BIO_f_base64());
  BIO_push(b64, bio);
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

  if((ret = BIO_write(b64, message, length)) != length) {
    fprintf(stderr,"BIO_write() return value is %d, while %d is expected\n",ret, length);
    exit(1);
  }

  ret = BIO_flush(b64);
  BIO_free_all(b64);
  fclose(stream);

  printf("Base64Encode(), \norigian length is %zu,origial:%s\nprocessed length is %zu processed:%s\n",length, message, strlen(buffer), buffer);

  return encodedSize;
}

int calcDecodeLength(const char* b64input) { //Calculates the length of a decoded base64 string
  int len = strlen(b64input);
  int padding = 0;
 
  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;
 
  return (int)len*0.75 - padding;
}

int Base64Decode(char* b64message, char* buffer, int length) { //Decodes a base64 encoded string
  BIO *bio, *b64;
  int ret=0;
  FILE* stream;

  if((stream = fmemopen(b64message, length, "r")) == NULL) {
    perror("fmemopen()");
    exit(1);
  }

  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  b64 = BIO_new(BIO_f_base64());
  BIO_push(b64, bio);
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer

  ret = BIO_read(b64, buffer, length);
    //Can test here if len == decodeLen - if not, then return an error
 
  BIO_free_all(b64);
  fclose(stream);

  printf("Base64Decode(), \norigian length is %zu,origial:%s\nprocessed length is %zu processed:%s\n",length, b64message, ret, buffer);

  if(length != (ret=calcDecodeLength(b64message))) {
    fprintf(stderr,"ERROR:processed length is %zu, which is wrong, it should be %d\n", length, ret);
   // exit(1);
  }
 
  return length; //success
}
