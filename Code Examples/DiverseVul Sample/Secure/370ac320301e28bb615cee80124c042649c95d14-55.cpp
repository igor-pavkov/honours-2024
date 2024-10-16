int X509_STORE_CTX_set_purpose(X509_STORE_CTX *ctx, int purpose)
{
    return X509_STORE_CTX_purpose_inherit(ctx, 0, purpose, 0);
}