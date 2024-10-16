void X509_STORE_CTX_trusted_stack(X509_STORE_CTX *ctx, STACK_OF(X509) *sk)
{
    ctx->other_ctx = sk;
    ctx->get_issuer = get_issuer_sk;
}