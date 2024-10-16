static void get_delta_sk(X509_STORE_CTX *ctx, X509_CRL **dcrl, int *pscore,
                         X509_CRL *base, STACK_OF(X509_CRL) *crls)
{
    X509_CRL *delta;
    int i;
    if (!(ctx->param->flags & X509_V_FLAG_USE_DELTAS))
        return;
    if (!((ctx->current_cert->ex_flags | base->flags) & EXFLAG_FRESHEST))
        return;
    for (i = 0; i < sk_X509_CRL_num(crls); i++) {
        delta = sk_X509_CRL_value(crls, i);
        if (check_delta_base(delta, base)) {
            if (check_crl_time(ctx, delta, 0))
                *pscore |= CRL_SCORE_TIME_DELTA;
            CRYPTO_add(&delta->references, 1, CRYPTO_LOCK_X509_CRL);
            *dcrl = delta;
            return;
        }
    }
    *dcrl = NULL;
}