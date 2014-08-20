#include "md5.h"
#include "md5c.h"
#include "hmacmd5.h"
#include <stdio.h>
#include <string.h>

void make_digest_ex(unsigned char *md5str, unsigned char *digest, int len)
{
  static const char hexits[17] = "0123456789abcdef";
  int i;

  for (i = 0; i < len; i++) {
    md5str[i * 2] = hexits[digest[i] >> 4];
    md5str[(i * 2) + 1] = hexits[digest[i] & 0x0F];
  }
  md5str[len * 2] = '\0';
}

void hmac_md5(const char *message, const char *enc_key, unsigned char *result)
{
    const unsigned char *data = message;
    const unsigned char *key_input = enc_key; 

    unsigned char digest[16];
   // unsigned char hexdigest[33];
    unsigned int i;

    MD5_CTX context;

    size_t keylen = strlen(key_input); 

    char ipad[64], opad[64];

    memset(digest, 0, 16);
    //memset(hexdigest, 0, 33);
    memset(result, 0, 32);
    memset(ipad, 0, 64);
    memset(opad, 0, 64);

    if(keylen > 64)
    {
        char optkeybuf[20];
        MD5_CTX keyhash;

        MD5Init(&keyhash);
        MD5Update(&keyhash, (unsigned char*) key_input, keylen);
        MD5Final((unsigned char*) optkeybuf, &keyhash);

        keylen = 20;
        key_input = optkeybuf;
    }

    memcpy(ipad, key_input, keylen);
    memcpy(opad, key_input, keylen);
    memset(ipad+keylen, 0, 64 - keylen);
    memset(opad+keylen, 0, 64 - keylen);

    for (i = 0; i < 64; i++)
    {
        ipad[i] ^= 0x36;
        opad[i] ^= 0x5c;
    }    

    MD5Init(&context);
    MD5Update(&context, (unsigned char*) ipad, 64);
    //MD5Update(&context, (unsigned char*) *data, data.length());
    MD5Update(&context, data, strlen(data));
    MD5Final(digest, &context);

    MD5Init(&context);
    MD5Update(&context, (unsigned char*) opad, 64);
    MD5Update(&context, digest, 16);
    MD5Final(digest, &context);

    make_digest_ex(result, digest, 16);
    //return scope.Close(String::New((char*)hexdigest,32));
   // return hexdigest;
} 