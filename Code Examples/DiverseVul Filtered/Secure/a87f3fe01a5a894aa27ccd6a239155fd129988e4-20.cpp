static int check_cert_usable(SSL *s, const SIGALG_LOOKUP *sig, X509 *x,
                             EVP_PKEY *pkey)
{
    const SIGALG_LOOKUP *lu;
    int mdnid, pknid, supported;
    size_t i;

    /*
     * If the given EVP_PKEY cannot supporting signing with this sigalg,
     * the answer is simply 'no'.
     */
    ERR_set_mark();
    supported = EVP_PKEY_supports_digest_nid(pkey, sig->hash);
    ERR_pop_to_mark();
    if (supported == 0)
        return 0;

    /*
     * The TLS 1.3 signature_algorithms_cert extension places restrictions
     * on the sigalg with which the certificate was signed (by its issuer).
     */
    if (s->s3.tmp.peer_cert_sigalgs != NULL) {
        if (!X509_get_signature_info(x, &mdnid, &pknid, NULL, NULL))
            return 0;
        for (i = 0; i < s->s3.tmp.peer_cert_sigalgslen; i++) {
            lu = tls1_lookup_sigalg(s->s3.tmp.peer_cert_sigalgs[i]);
            if (lu == NULL)
                continue;

            /*
             * TODO this does not differentiate between the
             * rsa_pss_pss_* and rsa_pss_rsae_* schemes since we do not
             * have a chain here that lets us look at the key OID in the
             * signing certificate.
             */
            if (mdnid == lu->hash && pknid == lu->sig)
                return 1;
        }
        return 0;
    }

    /*
     * Without signat_algorithms_cert, any certificate for which we have
     * a viable public key is permitted.
     */
    return 1;
}