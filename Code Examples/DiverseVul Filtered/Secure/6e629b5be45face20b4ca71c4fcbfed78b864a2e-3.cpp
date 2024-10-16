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
         * Proxy certificates policy has an extra constraint, where the
         * certificate subject MUST be the issuer with a single CN entry
         * added.
         * (RFC 3820: 3.4, 4.1.3 (a)(4))
         */
        if (x->ex_flags & EXFLAG_PROXY) {
            X509_NAME *tmpsubject = X509_get_subject_name(x);
            X509_NAME *tmpissuer = X509_get_issuer_name(x);
            X509_NAME_ENTRY *tmpentry = NULL;
            int last_object_nid = 0;
            int err = X509_V_OK;
            int last_object_loc = X509_NAME_entry_count(tmpsubject) - 1;

            /* Check that there are at least two RDNs */
            if (last_object_loc < 1) {
                err = X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION;
                goto proxy_name_done;
            }

            /*
             * Check that there is exactly one more RDN in subject as
             * there is in issuer.
             */
            if (X509_NAME_entry_count(tmpsubject)
                != X509_NAME_entry_count(tmpissuer) + 1) {
                err = X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION;
                goto proxy_name_done;
            }

            /*
             * Check that the last subject component isn't part of a
             * multivalued RDN
             */
            if (X509_NAME_get_entry(tmpsubject, last_object_loc)->set
                == X509_NAME_get_entry(tmpsubject, last_object_loc - 1)->set) {
                err = X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION;
                goto proxy_name_done;
            }

            /*
             * Check that the last subject RDN is a commonName, and that
             * all the previous RDNs match the issuer exactly
             */
            tmpsubject = X509_NAME_dup(tmpsubject);
            if (tmpsubject == NULL) {
                X509err(X509_F_CHECK_NAME_CONSTRAINTS, ERR_R_MALLOC_FAILURE);
                ctx->error = X509_V_ERR_OUT_OF_MEM;
                return 0;
            }

            tmpentry =
                X509_NAME_delete_entry(tmpsubject, last_object_loc);
            last_object_nid =
                OBJ_obj2nid(X509_NAME_ENTRY_get_object(tmpentry));

            if (last_object_nid != NID_commonName
                || X509_NAME_cmp(tmpsubject, tmpissuer) != 0) {
                err = X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION;
            }

            X509_NAME_ENTRY_free(tmpentry);
            X509_NAME_free(tmpsubject);

         proxy_name_done:
            if (err != X509_V_OK) {
                ctx->error = err;
                ctx->error_depth = i;
                ctx->current_cert = x;
                if (!ctx->verify_cb(0, ctx))
                    return 0;
            }
        }

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
                switch (rv) {
                case X509_V_OK:
                    continue;
                case X509_V_ERR_OUT_OF_MEM:
                    ctx->error = rv;
                    return 0;
                default:
                    ctx->error = rv;
                    ctx->error_depth = i;
                    ctx->current_cert = x;
                    if (!ctx->verify_cb(0, ctx))
                        return 0;
                    break;
                }
            }
        }
    }
    return 1;
}