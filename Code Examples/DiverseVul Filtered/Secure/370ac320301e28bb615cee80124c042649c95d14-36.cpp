static int check_name_constraints(X509_STORE_CTX *ctx)
{
    X509 *x;
    int i, j, rv;
    /* Check name constraints for all certificates */
    for (i = sk_X509_num(ctx->chain) - 1; i >= 0; i--) {
        x = sk_X509_value(ctx->chain, i);
        /* Ignore self issued certs unless last in chain */
        if (i && (x->ex_flags & EXFLAG_SI))
            continue;
        /*
         * Check against constraints for all certificates higher in chain
         * including trust anchor. Trust anchor not strictly speaking needed
         * but if it includes constraints it is to be assumed it expects them
         * to be obeyed.
         */
        for (j = sk_X509_num(ctx->chain) - 1; j > i; j--) {
            NAME_CONSTRAINTS *nc = sk_X509_value(ctx->chain, j)->nc;
            if (nc) {
                rv = NAME_CONSTRAINTS_check(x, nc);
                if (rv != X509_V_OK) {
                    ctx->error = rv;
                    ctx->error_depth = i;
                    ctx->current_cert = x;
                    if (!ctx->verify_cb(0, ctx))
                        return 0;
                }
            }
        }
    }
    return 1;
}