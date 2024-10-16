int X509_STORE_CTX_set_trust(X509_STORE_CTX *ctx, int trust)
{
    return X509_STORE_CTX_purpose_inherit(ctx, 0, 0, trust);
}