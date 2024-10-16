STACK_OF(X509) *X509_STORE_CTX_get1_chain(X509_STORE_CTX *ctx)
{
    int i;
    X509 *x;
    STACK_OF(X509) *chain;
    if (!ctx->chain || !(chain = sk_X509_dup(ctx->chain)))
        return NULL;
    for (i = 0; i < sk_X509_num(chain); i++) {
        x = sk_X509_value(chain, i);
        CRYPTO_add(&x->references, 1, CRYPTO_LOCK_X509);
    }
    return chain;
}