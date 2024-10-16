static int tls1_check_cert_param(SSL *s, X509 *x, int check_ee_md)
{
    uint16_t group_id;
    EVP_PKEY *pkey;
    pkey = X509_get0_pubkey(x);
    if (pkey == NULL)
        return 0;
    /* If not EC nothing to do */
    if (!EVP_PKEY_is_a(pkey, "EC"))
        return 1;
    /* Check compression */
    if (!tls1_check_pkey_comp(s, pkey))
        return 0;
    group_id = tls1_get_group_id(pkey);
    /*
     * For a server we allow the certificate to not be in our list of supported
     * groups.
     */
    if (!tls1_check_group_id(s, group_id, !s->server))
        return 0;
    /*
     * Special case for suite B. We *MUST* sign using SHA256+P-256 or
     * SHA384+P-384.
     */
    if (check_ee_md && tls1_suiteb(s)) {
        int check_md;
        size_t i;

        /* Check to see we have necessary signing algorithm */
        if (group_id == TLSEXT_curve_P_256)
            check_md = NID_ecdsa_with_SHA256;
        else if (group_id == TLSEXT_curve_P_384)
            check_md = NID_ecdsa_with_SHA384;
        else
            return 0;           /* Should never happen */
        for (i = 0; i < s->shared_sigalgslen; i++) {
            if (check_md == s->shared_sigalgs[i]->sigandhash)
                return 1;;
        }
        return 0;
    }
    return 1;
}