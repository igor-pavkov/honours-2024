static int get_issuer_sk(X509 **issuer, X509_STORE_CTX *ctx, X509 *x)
{
    *issuer = find_issuer(ctx, ctx->other_ctx, x);
    if (*issuer) {
        CRYPTO_add(&(*issuer)->references, 1, CRYPTO_LOCK_X509);
        return 1;
    } else
        return 0;
}