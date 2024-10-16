static void *rc4_hmac_md5_newctx(void *provctx)
{
    PROV_RC4_HMAC_MD5_CTX *ctx;

    if (!ossl_prov_is_running())
        return NULL;

    ctx = OPENSSL_zalloc(sizeof(*ctx));
    if (ctx != NULL)
        ossl_cipher_generic_initkey(ctx, RC4_HMAC_MD5_KEY_BITS,
                                    RC4_HMAC_MD5_BLOCK_BITS,
                                    RC4_HMAC_MD5_IV_BITS,
                                    RC4_HMAC_MD5_MODE, RC4_HMAC_MD5_FLAGS,
                                    ossl_prov_cipher_hw_rc4_hmac_md5(
                                        RC4_HMAC_MD5_KEY_BITS
                                    ), NULL);
     return ctx;
}