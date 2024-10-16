static int crldp_check_crlissuer(DIST_POINT *dp, X509_CRL *crl, int crl_score)
{
    int i;
    X509_NAME *nm = X509_CRL_get_issuer(crl);
    /* If no CRLissuer return is successful iff don't need a match */
    if (!dp->CRLissuer)
        return ! !(crl_score & CRL_SCORE_ISSUER_NAME);
    for (i = 0; i < sk_GENERAL_NAME_num(dp->CRLissuer); i++) {
        GENERAL_NAME *gen = sk_GENERAL_NAME_value(dp->CRLissuer, i);
        if (gen->type != GEN_DIRNAME)
            continue;
        if (!X509_NAME_cmp(gen->d.directoryName, nm))
            return 1;
    }
    return 0;
}