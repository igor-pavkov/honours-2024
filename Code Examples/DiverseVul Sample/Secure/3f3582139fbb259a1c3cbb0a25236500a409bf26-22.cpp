int EVP_CipherInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher,
                   const unsigned char *key, const unsigned char *iv, int enc)
{
    EVP_CIPHER_CTX_reset(ctx);
    return EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, enc);
}