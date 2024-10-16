static int check_cert_time(X509_STORE_CTX *ctx, X509 *x)
{
    time_t *ptime;
    int i;

    if (ctx->param->flags & X509_V_FLAG_USE_CHECK_TIME)
        ptime = &ctx->param->check_time;
    else
        ptime = NULL;

    i = X509_cmp_time(X509_get_notBefore(x), ptime);
    if (i == 0) {
        ctx->error = X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD;
        ctx->current_cert = x;
        if (!ctx->verify_cb(0, ctx))
            return 0;
    }

    if (i > 0) {
        ctx->error = X509_V_ERR_CERT_NOT_YET_VALID;
        ctx->current_cert = x;
        if (!ctx->verify_cb(0, ctx))
            return 0;
    }

    i = X509_cmp_time(X509_get_notAfter(x), ptime);
    if (i == 0) {
        ctx->error = X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD;
        ctx->current_cert = x;
        if (!ctx->verify_cb(0, ctx))
            return 0;
    }

    if (i < 0) {
        ctx->error = X509_V_ERR_CERT_HAS_EXPIRED;
        ctx->current_cert = x;
        if (!ctx->verify_cb(0, ctx))
            return 0;
    }

    return 1;
}