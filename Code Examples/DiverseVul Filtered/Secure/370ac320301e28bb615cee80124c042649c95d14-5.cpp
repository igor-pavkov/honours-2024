static int check_crl(X509_STORE_CTX *ctx, X509_CRL *crl)
{
    X509 *issuer = NULL;
    EVP_PKEY *ikey = NULL;
    int ok = 0, chnum, cnum;
    cnum = ctx->error_depth;
    chnum = sk_X509_num(ctx->chain) - 1;
    /* if we have an alternative CRL issuer cert use that */
    if (ctx->current_issuer)
        issuer = ctx->current_issuer;

    /*
     * Else find CRL issuer: if not last certificate then issuer is next
     * certificate in chain.
     */
    else if (cnum < chnum)
        issuer = sk_X509_value(ctx->chain, cnum + 1);
    else {
        issuer = sk_X509_value(ctx->chain, chnum);
        /* If not self signed, can't check signature */
        if (!ctx->check_issued(ctx, issuer, issuer)) {
            ctx->error = X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER;
            ok = ctx->verify_cb(0, ctx);
            if (!ok)
                goto err;
        }
    }

    if (issuer) {
        /*
         * Skip most tests for deltas because they have already been done
         */
        if (!crl->base_crl_number) {
            /* Check for cRLSign bit if keyUsage present */
            if ((issuer->ex_flags & EXFLAG_KUSAGE) &&
                !(issuer->ex_kusage & KU_CRL_SIGN)) {
                ctx->error = X509_V_ERR_KEYUSAGE_NO_CRL_SIGN;
                ok = ctx->verify_cb(0, ctx);
                if (!ok)
                    goto err;
            }

            if (!(ctx->current_crl_score & CRL_SCORE_SCOPE)) {
                ctx->error = X509_V_ERR_DIFFERENT_CRL_SCOPE;
                ok = ctx->verify_cb(0, ctx);
                if (!ok)
                    goto err;
            }

            if (!(ctx->current_crl_score & CRL_SCORE_SAME_PATH)) {
                if (check_crl_path(ctx, ctx->current_issuer) <= 0) {
                    ctx->error = X509_V_ERR_CRL_PATH_VALIDATION_ERROR;
                    ok = ctx->verify_cb(0, ctx);
                    if (!ok)
                        goto err;
                }
            }

            if (crl->idp_flags & IDP_INVALID) {
                ctx->error = X509_V_ERR_INVALID_EXTENSION;
                ok = ctx->verify_cb(0, ctx);
                if (!ok)
                    goto err;
            }

        }

        if (!(ctx->current_crl_score & CRL_SCORE_TIME)) {
            ok = check_crl_time(ctx, crl, 1);
            if (!ok)
                goto err;
        }

        /* Attempt to get issuer certificate public key */
        ikey = X509_get_pubkey(issuer);

        if (!ikey) {
            ctx->error = X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY;
            ok = ctx->verify_cb(0, ctx);
            if (!ok)
                goto err;
        } else {
            /* Verify CRL signature */
            if (X509_CRL_verify(crl, ikey) <= 0) {
                ctx->error = X509_V_ERR_CRL_SIGNATURE_FAILURE;
                ok = ctx->verify_cb(0, ctx);
                if (!ok)
                    goto err;
            }
        }
    }

    ok = 1;

 err:
    EVP_PKEY_free(ikey);
    return ok;
}