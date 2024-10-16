int EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
{
    int ret;
    ret = EVP_EncryptFinal_ex(ctx, out, outl);
    return ret;
}