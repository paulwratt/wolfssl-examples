#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/pkcs7.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef HAVE_PKCS7

int main(int argc, char** argv)
{
    int rc = 0;
    PKCS7 pkcs7;
    XFILE derFile;
    byte* derBuf = NULL;
    word32 derSz = 0;

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    /* load DER PKCS7 */
    derFile = fopen("signed.p7s", "rb");
    if (derFile) {
        fseek(derFile, 0, SEEK_END);
        derSz = (int)ftell(derFile);
        rewind(derFile);

        derBuf = (byte*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (derBuf == NULL) {
            rc = MEMORY_E; goto exit;
        }

        rc = (int)fread(derBuf, 1, derSz, derFile);
        fclose(derFile);
    }

    printf("Der %d\n", derSz);
    WOLFSSL_BUFFER(derBuf, derSz);

    /* Test verify */
    rc = wc_PKCS7_Init(&pkcs7, NULL, INVALID_DEVID);
    if (rc != 0) goto exit;
    rc = wc_PKCS7_InitWithCert(&pkcs7, NULL, 0);
    if (rc != 0) goto exit;
    rc = wc_PKCS7_VerifySignedData(&pkcs7, derBuf, derSz);
    if (rc != 0) goto exit;

    printf("PKCS7 Verify Success\n");

exit:

    if (rc != 0)
        printf("RC=%d\n", rc);

    wc_PKCS7_Free(&pkcs7);
    XFREE(derBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return rc;
}

#else

int main(int argc, char** argv)
{
    printf("Not compiled in: Must build wolfSSL using ./configure --enable-pkcs7\n");
    return 0;
}

#endif
