static int final_ec_pt_formats(SSL *s, unsigned int context, int sent)
{
    unsigned long alg_k, alg_a;

    if (s->server)
        return 1;

    alg_k = s->s3->tmp.new_cipher->algorithm_mkey;
    alg_a = s->s3->tmp.new_cipher->algorithm_auth;

    /*
     * If we are client and using an elliptic curve cryptography cipher
     * suite, then if server returns an EC point formats lists extension it
     * must contain uncompressed.
     */
    if (s->ext.ecpointformats != NULL
            && s->ext.ecpointformats_len > 0
            && s->ext.peer_ecpointformats != NULL
            && s->ext.peer_ecpointformats_len > 0
            && ((alg_k & SSL_kECDHE) || (alg_a & SSL_aECDSA))) {
        /* we are using an ECC cipher */
        size_t i;
        unsigned char *list = s->ext.peer_ecpointformats;

        for (i = 0; i < s->ext.peer_ecpointformats_len; i++) {
            if (*list++ == TLSEXT_ECPOINTFORMAT_uncompressed)
                break;
        }
        if (i == s->ext.peer_ecpointformats_len) {
            SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_F_FINAL_EC_PT_FORMATS,
                     SSL_R_TLS_INVALID_ECPOINTFORMAT_LIST);
            return 0;
        }
    }

    return 1;
}