int X509_STORE_CTX_set_default(X509_STORE_CTX *ctx, const char *name)
{
    const X509_VERIFY_PARAM *param;
    param = X509_VERIFY_PARAM_lookup(name);
    if (!param)
        return 0;
    return X509_VERIFY_PARAM_inherit(ctx->param, param);
}