void *X509_STORE_CTX_get_ex_data(X509_STORE_CTX *ctx, int idx)
{
    return CRYPTO_get_ex_data(&ctx->ex_data, idx);
}