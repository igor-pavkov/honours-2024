SSL *SSL_dup(SSL *s)
{
    STACK_OF(X509_NAME) *sk;
    X509_NAME *xn;
    SSL *ret;
    int i;

    if ((ret = SSL_new(SSL_get_SSL_CTX(s))) == NULL)
        return (NULL);

    ret->version = s->version;
    ret->type = s->type;
    ret->method = s->method;

    if (s->session != NULL) {
        /* This copies session-id, SSL_METHOD, sid_ctx, and 'cert' */
        SSL_copy_session_id(ret, s);
    } else {
        /*
         * No session has been established yet, so we have to expect that
         * s->cert or ret->cert will be changed later -- they should not both
         * point to the same object, and thus we can't use
         * SSL_copy_session_id.
         */

        ret->method->ssl_free(ret);
        ret->method = s->method;
        ret->method->ssl_new(ret);

        if (s->cert != NULL) {
            if (ret->cert != NULL) {
                ssl_cert_free(ret->cert);
            }
            ret->cert = ssl_cert_dup(s->cert);
            if (ret->cert == NULL)
                goto err;
        }

        SSL_set_session_id_context(ret, s->sid_ctx, s->sid_ctx_length);
    }

    ret->options = s->options;
    ret->mode = s->mode;
    SSL_set_max_cert_list(ret, SSL_get_max_cert_list(s));
    SSL_set_read_ahead(ret, SSL_get_read_ahead(s));
    ret->msg_callback = s->msg_callback;
    ret->msg_callback_arg = s->msg_callback_arg;
    SSL_set_verify(ret, SSL_get_verify_mode(s), SSL_get_verify_callback(s));
    SSL_set_verify_depth(ret, SSL_get_verify_depth(s));
    ret->generate_session_id = s->generate_session_id;

    SSL_set_info_callback(ret, SSL_get_info_callback(s));

    ret->debug = s->debug;

    /* copy app data, a little dangerous perhaps */
    if (!CRYPTO_dup_ex_data(CRYPTO_EX_INDEX_SSL, &ret->ex_data, &s->ex_data))
        goto err;

    /* setup rbio, and wbio */
    if (s->rbio != NULL) {
        if (!BIO_dup_state(s->rbio, (char *)&ret->rbio))
            goto err;
    }
    if (s->wbio != NULL) {
        if (s->wbio != s->rbio) {
            if (!BIO_dup_state(s->wbio, (char *)&ret->wbio))
                goto err;
        } else
            ret->wbio = ret->rbio;
    }
    ret->rwstate = s->rwstate;
    ret->in_handshake = s->in_handshake;
    ret->handshake_func = s->handshake_func;
    ret->server = s->server;
    ret->renegotiate = s->renegotiate;
    ret->new_session = s->new_session;
    ret->quiet_shutdown = s->quiet_shutdown;
    ret->shutdown = s->shutdown;
    ret->state = s->state;      /* SSL_dup does not really work at any state,
                                 * though */
    ret->rstate = s->rstate;
    ret->init_num = 0;          /* would have to copy ret->init_buf,
                                 * ret->init_msg, ret->init_num,
                                 * ret->init_off */
    ret->hit = s->hit;

    X509_VERIFY_PARAM_inherit(ret->param, s->param);

    /* dup the cipher_list and cipher_list_by_id stacks */
    if (s->cipher_list != NULL) {
        if ((ret->cipher_list = sk_SSL_CIPHER_dup(s->cipher_list)) == NULL)
            goto err;
    }
    if (s->cipher_list_by_id != NULL)
        if ((ret->cipher_list_by_id = sk_SSL_CIPHER_dup(s->cipher_list_by_id))
            == NULL)
            goto err;

    /* Dup the client_CA list */
    if (s->client_CA != NULL) {
        if ((sk = sk_X509_NAME_dup(s->client_CA)) == NULL)
            goto err;
        ret->client_CA = sk;
        for (i = 0; i < sk_X509_NAME_num(sk); i++) {
            xn = sk_X509_NAME_value(sk, i);
            if (sk_X509_NAME_set(sk, i, X509_NAME_dup(xn)) == NULL) {
                X509_NAME_free(xn);
                goto err;
            }
        }
    }

    if (0) {
 err:
        if (ret != NULL)
            SSL_free(ret);
        ret = NULL;
    }
    return (ret);
}