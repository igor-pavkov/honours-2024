int tls_psk_do_binder(SSL *s, const EVP_MD *md, const unsigned char *msgstart,
                      size_t binderoffset, const unsigned char *binderin,
                      unsigned char *binderout, SSL_SESSION *sess, int sign,
                      int external)
{
    EVP_PKEY *mackey = NULL;
    EVP_MD_CTX *mctx = NULL;
    unsigned char hash[EVP_MAX_MD_SIZE], binderkey[EVP_MAX_MD_SIZE];
    unsigned char finishedkey[EVP_MAX_MD_SIZE], tmpbinder[EVP_MAX_MD_SIZE];
    unsigned char *early_secret;
#ifdef CHARSET_EBCDIC
    static const unsigned char resumption_label[] = { 0x72, 0x65, 0x73, 0x20, 0x62, 0x69, 0x6E, 0x64, 0x65, 0x72, 0x00 };
    static const unsigned char external_label[]   = { 0x65, 0x78, 0x74, 0x20, 0x62, 0x69, 0x6E, 0x64, 0x65, 0x72, 0x00 };
#else
    static const unsigned char resumption_label[] = "res binder";
    static const unsigned char external_label[] = "ext binder";
#endif
    const unsigned char *label;
    size_t bindersize, labelsize, hashsize;
    int hashsizei = EVP_MD_size(md);
    int ret = -1;
    int usepskfored = 0;

    /* Ensure cast to size_t is safe */
    if (!ossl_assert(hashsizei >= 0)) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                 ERR_R_INTERNAL_ERROR);
        goto err;
    }
    hashsize = (size_t)hashsizei;

    if (external
            && s->early_data_state == SSL_EARLY_DATA_CONNECTING
            && s->session->ext.max_early_data == 0
            && sess->ext.max_early_data > 0)
        usepskfored = 1;

    if (external) {
        label = external_label;
        labelsize = sizeof(external_label) - 1;
    } else {
        label = resumption_label;
        labelsize = sizeof(resumption_label) - 1;
    }

    /*
     * Generate the early_secret. On the server side we've selected a PSK to
     * resume with (internal or external) so we always do this. On the client
     * side we do this for a non-external (i.e. resumption) PSK or external PSK
     * that will be used for early_data so that it is in place for sending early
     * data. For client side external PSK not being used for early_data we
     * generate it but store it away for later use.
     */
    if (s->server || !external || usepskfored)
        early_secret = (unsigned char *)s->early_secret;
    else
        early_secret = (unsigned char *)sess->early_secret;

    if (!tls13_generate_secret(s, md, NULL, sess->master_key,
                               sess->master_key_length, early_secret)) {
        /* SSLfatal() already called */
        goto err;
    }

    /*
     * Create the handshake hash for the binder key...the messages so far are
     * empty!
     */
    mctx = EVP_MD_CTX_new();
    if (mctx == NULL
            || EVP_DigestInit_ex(mctx, md, NULL) <= 0
            || EVP_DigestFinal_ex(mctx, hash, NULL) <= 0) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                 ERR_R_INTERNAL_ERROR);
        goto err;
    }

    /* Generate the binder key */
    if (!tls13_hkdf_expand(s, md, early_secret, label, labelsize, hash,
                           hashsize, binderkey, hashsize, 1)) {
        /* SSLfatal() already called */
        goto err;
    }

    /* Generate the finished key */
    if (!tls13_derive_finishedkey(s, md, binderkey, finishedkey, hashsize)) {
        /* SSLfatal() already called */
        goto err;
    }

    if (EVP_DigestInit_ex(mctx, md, NULL) <= 0) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                 ERR_R_INTERNAL_ERROR);
        goto err;
    }

    /*
     * Get a hash of the ClientHello up to the start of the binders. If we are
     * following a HelloRetryRequest then this includes the hash of the first
     * ClientHello and the HelloRetryRequest itself.
     */
    if (s->hello_retry_request == SSL_HRR_PENDING) {
        size_t hdatalen;
        long hdatalen_l;
        void *hdata;

        hdatalen = hdatalen_l =
            BIO_get_mem_data(s->s3->handshake_buffer, &hdata);
        if (hdatalen_l <= 0) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                     SSL_R_BAD_HANDSHAKE_LENGTH);
            goto err;
        }

        /*
         * For servers the handshake buffer data will include the second
         * ClientHello - which we don't want - so we need to take that bit off.
         */
        if (s->server) {
            PACKET hashprefix, msg;

            /* Find how many bytes are left after the first two messages */
            if (!PACKET_buf_init(&hashprefix, hdata, hdatalen)
                    || !PACKET_forward(&hashprefix, 1)
                    || !PACKET_get_length_prefixed_3(&hashprefix, &msg)
                    || !PACKET_forward(&hashprefix, 1)
                    || !PACKET_get_length_prefixed_3(&hashprefix, &msg)) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                         ERR_R_INTERNAL_ERROR);
                goto err;
            }
            hdatalen -= PACKET_remaining(&hashprefix);
        }

        if (EVP_DigestUpdate(mctx, hdata, hdatalen) <= 0) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                     ERR_R_INTERNAL_ERROR);
            goto err;
        }
    }

    if (EVP_DigestUpdate(mctx, msgstart, binderoffset) <= 0
            || EVP_DigestFinal_ex(mctx, hash, NULL) <= 0) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                 ERR_R_INTERNAL_ERROR);
        goto err;
    }

    mackey = EVP_PKEY_new_raw_private_key(EVP_PKEY_HMAC, NULL, finishedkey,
                                          hashsize);
    if (mackey == NULL) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                 ERR_R_INTERNAL_ERROR);
        goto err;
    }

    if (!sign)
        binderout = tmpbinder;

    bindersize = hashsize;
    if (EVP_DigestSignInit(mctx, NULL, md, NULL, mackey) <= 0
            || EVP_DigestSignUpdate(mctx, hash, hashsize) <= 0
            || EVP_DigestSignFinal(mctx, binderout, &bindersize) <= 0
            || bindersize != hashsize) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_PSK_DO_BINDER,
                 ERR_R_INTERNAL_ERROR);
        goto err;
    }

    if (sign) {
        ret = 1;
    } else {
        /* HMAC keys can't do EVP_DigestVerify* - use CRYPTO_memcmp instead */
        ret = (CRYPTO_memcmp(binderin, binderout, hashsize) == 0);
        if (!ret)
            SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_F_TLS_PSK_DO_BINDER,
                     SSL_R_BINDER_DOES_NOT_VERIFY);
    }

 err:
    OPENSSL_cleanse(binderkey, sizeof(binderkey));
    OPENSSL_cleanse(finishedkey, sizeof(finishedkey));
    EVP_PKEY_free(mackey);
    EVP_MD_CTX_free(mctx);

    return ret;
}