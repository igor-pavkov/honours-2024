static int get_crl_delta(X509_STORE_CTX *ctx,
                         X509_CRL **pcrl, X509_CRL **pdcrl, X509 *x)
{
    int ok;
    X509 *issuer = NULL;
    int crl_score = 0;
    unsigned int reasons;
    X509_CRL *crl = NULL, *dcrl = NULL;
    STACK_OF(X509_CRL) *skcrl;
    X509_NAME *nm = X509_get_issuer_name(x);
    reasons = ctx->current_reasons;
    ok = get_crl_sk(ctx, &crl, &dcrl,
                    &issuer, &crl_score, &reasons, ctx->crls);

    if (ok)
        goto done;

    /* Lookup CRLs from store */

    skcrl = ctx->lookup_crls(ctx, nm);

    /* If no CRLs found and a near match from get_crl_sk use that */
    if (!skcrl && crl)
        goto done;

    get_crl_sk(ctx, &crl, &dcrl, &issuer, &crl_score, &reasons, skcrl);

    sk_X509_CRL_pop_free(skcrl, X509_CRL_free);

 done:

    /* If we got any kind of CRL use it and return success */
    if (crl) {
        ctx->current_issuer = issuer;
        ctx->current_crl_score = crl_score;
        ctx->current_reasons = reasons;
        *pcrl = crl;
        *pdcrl = dcrl;
        return 1;
    }

    return 0;
}