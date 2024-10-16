static int check_issued(X509_STORE_CTX *ctx, X509 *x, X509 *issuer)
{
    int ret;
    ret = X509_check_issued(issuer, x);
    if (ret == X509_V_OK)
        return 1;
    /* If we haven't asked for issuer errors don't set ctx */
    if (!(ctx->param->flags & X509_V_FLAG_CB_ISSUER_CHECK))
        return 0;

    ctx->error = ret;
    ctx->current_cert = x;
    ctx->current_issuer = issuer;
    return ctx->verify_cb(0, ctx);
    return 0;
}