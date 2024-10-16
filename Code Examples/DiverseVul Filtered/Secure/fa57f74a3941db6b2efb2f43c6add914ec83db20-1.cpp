static int check_policy(X509_STORE_CTX *ctx)
{
    int ret;
    ret = X509_policy_check(&ctx->tree, &ctx->explicit_policy, ctx->chain,
                            ctx->param->policies, ctx->param->flags);
    if (ret == 0) {
        X509err(X509_F_CHECK_POLICY, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    /* Invalid or inconsistent extensions */
    if (ret == -1) {
        /*
         * Locate certificates with bad extensions and notify callback.
         */
        X509 *x;
        int i;
        for (i = 1; i < sk_X509_num(ctx->chain); i++) {
            x = sk_X509_value(ctx->chain, i);
            if (!(x->ex_flags & EXFLAG_INVALID_POLICY))
                continue;
            ctx->current_cert = x;
            ctx->error = X509_V_ERR_INVALID_POLICY_EXTENSION;
            ret = ctx->verify_cb(0, ctx);
        }
        return 1;
    }
    if (ret == -2) {
        ctx->current_cert = NULL;
        ctx->error = X509_V_ERR_NO_EXPLICIT_POLICY;
        return ctx->verify_cb(0, ctx);
    }

    if (ctx->param->flags & X509_V_FLAG_NOTIFY_POLICY) {
        ctx->current_cert = NULL;
        ctx->error = X509_V_OK;
        if (!ctx->verify_cb(2, ctx))
            return 0;
    }

    return 1;
}