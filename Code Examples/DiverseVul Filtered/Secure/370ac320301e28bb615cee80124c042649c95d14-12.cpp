static int check_crl_path(X509_STORE_CTX *ctx, X509 *x)
{
    X509_STORE_CTX crl_ctx;
    int ret;
    /* Don't allow recursive CRL path validation */
    if (ctx->parent)
        return 0;
    if (!X509_STORE_CTX_init(&crl_ctx, ctx->ctx, x, ctx->untrusted))
        return -1;

    crl_ctx.crls = ctx->crls;
    /* Copy verify params across */
    X509_STORE_CTX_set0_param(&crl_ctx, ctx->param);

    crl_ctx.parent = ctx;
    crl_ctx.verify_cb = ctx->verify_cb;

    /* Verify CRL issuer */
    ret = X509_verify_cert(&crl_ctx);

    if (ret <= 0)
        goto err;

    /* Check chain is acceptable */

    ret = check_crl_chain(ctx, ctx->chain, crl_ctx.chain);
 err:
    X509_STORE_CTX_cleanup(&crl_ctx);
    return ret;
}