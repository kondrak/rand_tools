#ifndef HMAC_MD5_H
#define HMAC_MD5_H

#ifdef __cplusplus
extern "C" {
#endif

    void hmac_md5(const char *message, const char *enc_key, unsigned char *result);

#ifdef __cplusplus
}
#endif

#endif