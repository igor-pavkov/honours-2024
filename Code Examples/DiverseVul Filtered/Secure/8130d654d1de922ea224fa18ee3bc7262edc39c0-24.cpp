int X509_chain_check_suiteb(int *perror_depth, X509 *x, STACK_OF(X509) *chain,
                            unsigned long flags)
{
    int rv, i, sign_nid;
    EVP_PKEY *pk;
    unsigned long tflags = flags;

    if (!(flags & X509_V_FLAG_SUITEB_128_LOS))
        return X509_V_OK;

    /* If no EE certificate passed in must be first in chain */
    if (x == NULL) {
        x = sk_X509_value(chain, 0);
        i = 1;
    } else {
        i = 0;
    }
    pk = X509_get0_pubkey(x);

    /*
     * With DANE-EE(3) success, or DANE-EE(3)/PKIX-EE(1) failure we don't build
     * a chain all, just report trust success or failure, but must also report
     * Suite-B errors if applicable.  This is indicated via a NULL chain
     * pointer.  All we need to do is check the leaf key algorithm.
     */
    if (chain == NULL)
        return check_suite_b(pk, -1, &tflags);

    if (X509_get_version(x) != 2) {
        rv = X509_V_ERR_SUITE_B_INVALID_VERSION;
        /* Correct error depth */
        i = 0;
        goto end;
    }

    /* Check EE key only */
    rv = check_suite_b(pk, -1, &tflags);
    if (rv != X509_V_OK) {
        /* Correct error depth */
        i = 0;
        goto end;
    }
    for (; i < sk_X509_num(chain); i++) {
        sign_nid = X509_get_signature_nid(x);
        x = sk_X509_value(chain, i);
        if (X509_get_version(x) != 2) {
            rv = X509_V_ERR_SUITE_B_INVALID_VERSION;
            goto end;
        }
        pk = X509_get0_pubkey(x);
        rv = check_suite_b(pk, sign_nid, &tflags);
        if (rv != X509_V_OK)
            goto end;
    }

    /* Final check: root CA signature */
    rv = check_suite_b(pk, X509_get_signature_nid(x), &tflags);
 end:
    if (rv != X509_V_OK) {
        /* Invalid signature or LOS errors are for previous cert */
        if ((rv == X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM
             || rv == X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED) && i)
            i--;
        /*
         * If we have LOS error and flags changed then we are signing P-384
         * with P-256. Use more meaningful error.
         */
        if (rv == X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED && flags != tflags)
            rv = X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256;
        if (perror_depth)
            *perror_depth = i;
    }
    return rv;
}