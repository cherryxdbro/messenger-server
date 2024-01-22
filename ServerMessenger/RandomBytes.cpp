#include "pch.h"

extern "C"
{
    #include "randombytes.h"
}

void randombytes(uint8_t* out, size_t outlen)
{
    HCRYPTPROV ctx;
    size_t len;
    if (!CryptAcquireContext(&ctx, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        abort();
    }
    while (outlen > 0)
    {
        len = (outlen > 1048576) ? 1048576 : outlen;
        if (!CryptGenRandom(ctx, static_cast<DWORD>(len), static_cast<BYTE*>(out)))
        {
            abort();
        }
        out += len;
        outlen -= len;
    }
    if (!CryptReleaseContext(ctx, 0))
    {
        abort();
    }
}
