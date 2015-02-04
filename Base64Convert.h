#ifndef BASE64CONVERT_H
#define BASE64CONVERT_H

extern int Base64Encode(const char* message, char* buffer, int length);

extern int Base64Decode(char* b64message, char* buffer, int length);

#endif
