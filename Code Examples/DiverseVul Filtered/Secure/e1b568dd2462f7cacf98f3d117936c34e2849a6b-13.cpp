int ssl3_get_server_hello(SSL *s)
{
    STACK_OF(SSL_CIPHER) *sk;
    const SSL_CIPHER *c;
    CERT *ct = s->cert;
    unsigned char *p, *d;
    int i, al = SSL_AD_INTERNAL_ERROR, ok;
    unsigned int j;
    long n;
#ifndef OPENSSL_NO_COMP
    SSL_COMP *comp;
#endif
    /*
     * Hello verify request and/or server hello version may not match so set
     * first packet if we're negotiating version.
     */
    if (SSL_IS_DTLS(s))
        s->first_packet = 1;

    n = s->method->ssl_get_message(s,
                                   SSL3_ST_CR_SRVR_HELLO_A,
                                   SSL3_ST_CR_SRVR_HELLO_B, -1, 20000, &ok);

    if (!ok)
        return ((int)n);

    if (SSL_IS_DTLS(s)) {
        s->first_packet = 0;
        if (s->s3->tmp.message_type == DTLS1_MT_HELLO_VERIFY_REQUEST) {
            if (s->d1->send_cookie == 0) {
                s->s3->tmp.reuse_message = 1;
                return 1;
            } else {            /* already sent a cookie */

                al = SSL_AD_UNEXPECTED_MESSAGE;
                SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_BAD_MESSAGE_TYPE);
                goto f_err;
            }
        }
    }

    if (s->s3->tmp.message_type != SSL3_MT_SERVER_HELLO) {
        al = SSL_AD_UNEXPECTED_MESSAGE;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_BAD_MESSAGE_TYPE);
        goto f_err;
    }

    d = p = (unsigned char *)s->init_msg;
    if (s->method->version == DTLS_ANY_VERSION) {
        /* Work out correct protocol version to use */
        int hversion = (p[0] << 8) | p[1];
        int options = s->options;
        if (hversion == DTLS1_2_VERSION && !(options & SSL_OP_NO_DTLSv1_2))
            s->method = DTLSv1_2_client_method();
        else if (tls1_suiteb(s)) {
            SSLerr(SSL_F_SSL3_GET_SERVER_HELLO,
                   SSL_R_ONLY_DTLS_1_2_ALLOWED_IN_SUITEB_MODE);
            s->version = hversion;
            al = SSL_AD_PROTOCOL_VERSION;
            goto f_err;
        } else if (hversion == DTLS1_VERSION && !(options & SSL_OP_NO_DTLSv1))
            s->method = DTLSv1_client_method();
        else {
            SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_WRONG_SSL_VERSION);
            s->version = hversion;
            al = SSL_AD_PROTOCOL_VERSION;
            goto f_err;
        }
        s->version = s->method->version;
    }

    if ((p[0] != (s->version >> 8)) || (p[1] != (s->version & 0xff))) {
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_WRONG_SSL_VERSION);
        s->version = (s->version & 0xff00) | p[1];
        al = SSL_AD_PROTOCOL_VERSION;
        goto f_err;
    }
    p += 2;

    /* load the server hello data */
    /* load the server random */
    memcpy(s->s3->server_random, p, SSL3_RANDOM_SIZE);
    p += SSL3_RANDOM_SIZE;

    s->hit = 0;

    /* get the session-id */
    j = *(p++);

    if ((j > sizeof s->session->session_id) || (j > SSL3_SESSION_ID_SIZE)) {
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_SSL3_SESSION_ID_TOO_LONG);
        goto f_err;
    }
#ifndef OPENSSL_NO_TLSEXT
    /*
     * check if we want to resume the session based on external pre-shared
     * secret
     */
    if (s->version >= TLS1_VERSION && s->tls_session_secret_cb) {
        SSL_CIPHER *pref_cipher = NULL;
        s->session->master_key_length = sizeof(s->session->master_key);
        if (s->tls_session_secret_cb(s, s->session->master_key,
                                     &s->session->master_key_length,
                                     NULL, &pref_cipher,
                                     s->tls_session_secret_cb_arg)) {
            s->session->cipher = pref_cipher ?
                pref_cipher : ssl_get_cipher_by_char(s, p + j);
            s->hit = 1;
        }
    }
#endif                          /* OPENSSL_NO_TLSEXT */

    if (!s->hit && j != 0 && j == s->session->session_id_length
        && memcmp(p, s->session->session_id, j) == 0) {
        if (s->sid_ctx_length != s->session->sid_ctx_length
            || memcmp(s->session->sid_ctx, s->sid_ctx, s->sid_ctx_length)) {
            /* actually a client application bug */
            al = SSL_AD_ILLEGAL_PARAMETER;
            SSLerr(SSL_F_SSL3_GET_SERVER_HELLO,
                   SSL_R_ATTEMPT_TO_REUSE_SESSION_IN_DIFFERENT_CONTEXT);
            goto f_err;
        }
        s->hit = 1;
    }
    /* a miss or crap from the other end */
    if (!s->hit) {
        /*
         * If we were trying for session-id reuse, make a new SSL_SESSION so
         * we don't stuff up other people
         */
        if (s->session->session_id_length > 0) {
            if (!ssl_get_new_session(s, 0)) {
                goto f_err;
            }
        }
        s->session->session_id_length = j;
        memcpy(s->session->session_id, p, j); /* j could be 0 */
    }
    p += j;
    c = ssl_get_cipher_by_char(s, p);
    if (c == NULL) {
        /* unknown cipher */
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_UNKNOWN_CIPHER_RETURNED);
        goto f_err;
    }
    /* Set version disabled mask now we know version */
    if (!SSL_USE_TLS1_2_CIPHERS(s))
        ct->mask_ssl = SSL_TLSV1_2;
    else
        ct->mask_ssl = 0;
    /*
     * If it is a disabled cipher we didn't send it in client hello, so
     * return an error.
     */
    if (ssl_cipher_disabled(s, c, SSL_SECOP_CIPHER_CHECK)) {
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_WRONG_CIPHER_RETURNED);
        goto f_err;
    }
    p += ssl_put_cipher_by_char(s, NULL, NULL);

    sk = ssl_get_ciphers_by_id(s);
    i = sk_SSL_CIPHER_find(sk, c);
    if (i < 0) {
        /* we did not say we would use this cipher */
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_WRONG_CIPHER_RETURNED);
        goto f_err;
    }

    /*
     * Depending on the session caching (internal/external), the cipher
     * and/or cipher_id values may not be set. Make sure that cipher_id is
     * set and use it for comparison.
     */
    if (s->session->cipher)
        s->session->cipher_id = s->session->cipher->id;
    if (s->hit && (s->session->cipher_id != c->id)) {
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO,
               SSL_R_OLD_SESSION_CIPHER_NOT_RETURNED);
        goto f_err;
    }
    s->s3->tmp.new_cipher = c;
    /*
     * Don't digest cached records if no sigalgs: we may need them for client
     * authentication.
     */
    if (!SSL_USE_SIGALGS(s) && !ssl3_digest_cached_records(s))
        goto f_err;
    /* lets get the compression algorithm */
    /* COMPRESSION */
#ifdef OPENSSL_NO_COMP
    if (*(p++) != 0) {
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO,
               SSL_R_UNSUPPORTED_COMPRESSION_ALGORITHM);
        goto f_err;
    }
    /*
     * If compression is disabled we'd better not try to resume a session
     * using compression.
     */
    if (s->session->compress_meth != 0) {
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_INCONSISTENT_COMPRESSION);
        goto f_err;
    }
#else
    j = *(p++);
    if (s->hit && j != s->session->compress_meth) {
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO,
               SSL_R_OLD_SESSION_COMPRESSION_ALGORITHM_NOT_RETURNED);
        goto f_err;
    }
    if (j == 0)
        comp = NULL;
    else if (!ssl_allow_compression(s)) {
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_COMPRESSION_DISABLED);
        goto f_err;
    } else
        comp = ssl3_comp_find(s->ctx->comp_methods, j);

    if ((j != 0) && (comp == NULL)) {
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO,
               SSL_R_UNSUPPORTED_COMPRESSION_ALGORITHM);
        goto f_err;
    } else {
        s->s3->tmp.new_compression = comp;
    }
#endif

#ifndef OPENSSL_NO_TLSEXT
    /* TLS extensions */
    if (!ssl_parse_serverhello_tlsext(s, &p, d, n)) {
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_PARSE_TLSEXT);
        goto err;
    }
#endif

    if (p != (d + n)) {
        /* wrong packet length */
        al = SSL_AD_DECODE_ERROR;
        SSLerr(SSL_F_SSL3_GET_SERVER_HELLO, SSL_R_BAD_PACKET_LENGTH);
        goto f_err;
    }

    return (1);
 f_err:
    ssl3_send_alert(s, SSL3_AL_FATAL, al);
 err:
    return (-1);
}