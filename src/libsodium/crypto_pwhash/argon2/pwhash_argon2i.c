
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "argon2.h"
#include "argon2-core.h"
#include "crypto_pwhash_argon2i.h"
#include "randombytes.h"
#include "utils.h"

#define STR_HASHBYTES 32U

size_t
crypto_pwhash_argon2i_saltbytes(void)
{
    return crypto_pwhash_argon2i_SALTBYTES;
}

size_t
crypto_pwhash_argon2i_strbytes(void)
{
    return crypto_pwhash_argon2i_STRBYTES;
}

const char *
crypto_pwhash_argon2i_strprefix(void)
{
    return crypto_pwhash_argon2i_STRPREFIX;
}

size_t
crypto_pwhash_argon2i_opslimit_interactive(void)
{
    return crypto_pwhash_argon2i_OPSLIMIT_INTERACTIVE;
}

size_t
crypto_pwhash_argon2i_memlimit_interactive(void)
{
    return crypto_pwhash_argon2i_MEMLIMIT_INTERACTIVE;
}

size_t
crypto_pwhash_argon2i_opslimit_moderate(void)
{
    return crypto_pwhash_argon2i_OPSLIMIT_MODERATE;
}

size_t
crypto_pwhash_argon2i_memlimit_moderate(void)
{
    return crypto_pwhash_argon2i_MEMLIMIT_MODERATE;
}

size_t
crypto_pwhash_argon2i_opslimit_sensitive(void)
{
    return crypto_pwhash_argon2i_OPSLIMIT_SENSITIVE;
}

size_t
crypto_pwhash_argon2i_memlimit_sensitive(void)
{
    return crypto_pwhash_argon2i_MEMLIMIT_SENSITIVE;
}

int
crypto_pwhash_argon2i(unsigned char * const out,
                      unsigned long long outlen,
                      const char * const passwd,
                      unsigned long long passwdlen,
                      const unsigned char * const salt,
                      unsigned long long opslimit,
                      size_t memlimit)
{
    memlimit /= 1024U;
    if (outlen > ARGON2_MAX_OUTLEN || passwdlen > ARGON2_MAX_PWD_LENGTH ||
        opslimit > ARGON2_MAX_TIME || memlimit > ARGON2_MAX_MEMORY) {
        errno = EFBIG;
        return -1;
    }
    if (outlen < ARGON2_MIN_OUTLEN || passwdlen < ARGON2_MIN_PWD_LENGTH ||
        opslimit < ARGON2_MIN_TIME || memlimit < ARGON2_MIN_MEMORY) {
        errno = EINVAL;
        return -1;
    }
    if (argon2i_hash_raw((uint32_t) opslimit, (uint32_t) memlimit,
                         (uint32_t) 1U, passwd, (size_t) passwdlen,
                         salt, (size_t) crypto_pwhash_argon2i_SALTBYTES,
                         out, (size_t) outlen) != ARGON2_OK) {
        return -1;
    }
    return 0;
}

int
crypto_pwhash_argon2i_str(char out[crypto_pwhash_argon2i_STRBYTES],
                          const char * const passwd,
                          unsigned long long passwdlen,
                          unsigned long long opslimit,
                          size_t memlimit)
{
    unsigned char salt[crypto_pwhash_argon2i_SALTBYTES];

    memlimit /= 1024U;
    if (passwdlen > ARGON2_MAX_PWD_LENGTH ||
        opslimit > ARGON2_MAX_TIME || memlimit > ARGON2_MAX_MEMORY) {
        errno = EFBIG;
        return -1;
    }
    if (passwdlen < ARGON2_MIN_PWD_LENGTH ||
        opslimit < ARGON2_MIN_TIME || memlimit < ARGON2_MIN_MEMORY) {
        errno = EINVAL;
        return -1;
    }
    randombytes_buf(salt, sizeof salt);
    memset(out, 0, crypto_pwhash_argon2i_STRBYTES);
    if (argon2i_hash_encoded((uint32_t) opslimit, (uint32_t) memlimit,
                             (uint32_t) 1U, passwd, (size_t) passwdlen,
                             salt, sizeof salt, STR_HASHBYTES,
                             out, crypto_pwhash_argon2i_STRBYTES) != ARGON2_OK) {
        return -1;
    }
    return 0;
}

int
crypto_pwhash_argon2i_str_verify(const char str[crypto_pwhash_argon2i_STRBYTES],
                                 const char * const passwd,
                                 unsigned long long passwdlen)
{
    if (passwdlen > ARGON2_MAX_PWD_LENGTH) {
        errno = EFBIG;
        return -1;
    }
    if (passwdlen < ARGON2_MIN_PWD_LENGTH) {
        errno = EINVAL;
        return -1;
    }
    if (argon2i_verify(str, passwd, (size_t) passwdlen) != ARGON2_OK) {
        return -1;
    }
    return 0;
}

int
_crypto_pwhash_argon2i_pick_best_implementation(void)
{
    return argon2_pick_best_implementation();
}
