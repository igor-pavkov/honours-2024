static int check_cert(X509_STORE_CTX *ctx)
{
    X509_CRL *crl = NULL;
    X509 *x;
    int ok, cnum;
    cnum = ctx->error_depth;
    x = sk_X509_value(ctx->chain, cnum);
    ctx->current_cert = x;
    /* Try to retrieve relevant CRL */
    ok = ctx->get_crl(ctx, &crl, x);
    /*
     * If error looking up CRL, nothing we can do except notify callback
     */
    if (!ok) {
        ctx->error = X509_V_ERR_UNABLE_TO_GET_CRL;
        ok = ctx->verify_cb(0, ctx);
        goto err;
    }
    ctx->current_crl = crl;
    ok = ctx->check_crl(ctx, crl);
    if (!ok)
        goto err;
    ok = ctx->cert_crl(ctx, crl, x);
 err:
    ctx->current_crl = NULL;
    X509_CRL_free(crl);
    return ok;

}