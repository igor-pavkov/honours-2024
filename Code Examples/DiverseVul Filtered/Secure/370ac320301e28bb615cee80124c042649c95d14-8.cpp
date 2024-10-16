void X509_STORE_CTX_set_verify_cb(X509_STORE_CTX *ctx,
                                  int (*verify_cb) (int, X509_STORE_CTX *))
{
    ctx->verify_cb = verify_cb;
}