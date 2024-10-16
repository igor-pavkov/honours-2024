static int check_crl_time(X509_STORE_CTX *ctx, X509_CRL *crl, int notify)
{
    time_t *ptime;
    int i;
    ctx->current_crl = crl;
    if (ctx->param->flags & X509_V_FLAG_USE_CHECK_TIME)
        ptime = &ctx->param->check_time;
    else
        ptime = NULL;

    i = X509_cmp_time(X509_CRL_get_lastUpdate(crl), ptime);
    if (i == 0) {
        ctx->error = X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD;
        if (!notify || !ctx->verify_cb(0, ctx))
            return 0;
    }

    if (i > 0) {
        ctx->error = X509_V_ERR_CRL_NOT_YET_VALID;
        if (!notify || !ctx->verify_cb(0, ctx))
            return 0;
    }

    if (X509_CRL_get_nextUpdate(crl)) {
        i = X509_cmp_time(X509_CRL_get_nextUpdate(crl), ptime);

        if (i == 0) {
            ctx->error = X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD;
            if (!notify || !ctx->verify_cb(0, ctx))
                return 0;
        }

        if (i < 0) {
            ctx->error = X509_V_ERR_CRL_HAS_EXPIRED;
            if (!notify || !ctx->verify_cb(0, ctx))
                return 0;
        }
    }

    ctx->current_crl = NULL;

    return 1;
}