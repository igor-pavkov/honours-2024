static int final_server_name(SSL *s, unsigned int context, int sent)
{
    int ret = SSL_TLSEXT_ERR_NOACK;
    int altmp = SSL_AD_UNRECOGNIZED_NAME;
    int was_ticket = (SSL_get_options(s) & SSL_OP_NO_TICKET) == 0;

    if (!ossl_assert(s->ctx != NULL) || !ossl_assert(s->session_ctx != NULL)) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_FINAL_SERVER_NAME,
                 ERR_R_INTERNAL_ERROR);
        return 0;
    }

    if (s->ctx->ext.servername_cb != NULL)
        ret = s->ctx->ext.servername_cb(s, &altmp,
                                        s->ctx->ext.servername_arg);
    else if (s->session_ctx->ext.servername_cb != NULL)
        ret = s->session_ctx->ext.servername_cb(s, &altmp,
                                       s->session_ctx->ext.servername_arg);

    /*
     * For servers, propagate the SNI hostname from the temporary
     * storage in the SSL to the persistent SSL_SESSION, now that we
     * know we accepted it.
     * Clients make this copy when parsing the server's response to
     * the extension, which is when they find out that the negotiation
     * was successful.
     */
    if (s->server) {
        if (sent && ret == SSL_TLSEXT_ERR_OK && !s->hit) {
            /* Only store the hostname in the session if we accepted it. */
            OPENSSL_free(s->session->ext.hostname);
            s->session->ext.hostname = OPENSSL_strdup(s->ext.hostname);
            if (s->session->ext.hostname == NULL && s->ext.hostname != NULL) {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_FINAL_SERVER_NAME,
                         ERR_R_INTERNAL_ERROR);
            }
        }
    }

    /*
     * If we switched contexts (whether here or in the client_hello callback),
     * move the sess_accept increment from the session_ctx to the new
     * context, to avoid the confusing situation of having sess_accept_good
     * exceed sess_accept (zero) for the new context.
     */
    if (SSL_IS_FIRST_HANDSHAKE(s) && s->ctx != s->session_ctx
		    && s->hello_retry_request == SSL_HRR_NONE) {
        tsan_counter(&s->ctx->stats.sess_accept);
        tsan_decr(&s->session_ctx->stats.sess_accept);
    }

    /*
     * If we're expecting to send a ticket, and tickets were previously enabled,
     * and now tickets are disabled, then turn off expected ticket.
     * Also, if this is not a resumption, create a new session ID
     */
    if (ret == SSL_TLSEXT_ERR_OK && s->ext.ticket_expected
            && was_ticket && (SSL_get_options(s) & SSL_OP_NO_TICKET) != 0) {
        s->ext.ticket_expected = 0;
        if (!s->hit) {
            SSL_SESSION* ss = SSL_get_session(s);

            if (ss != NULL) {
                OPENSSL_free(ss->ext.tick);
                ss->ext.tick = NULL;
                ss->ext.ticklen = 0;
                ss->ext.tick_lifetime_hint = 0;
                ss->ext.tick_age_add = 0;
                if (!ssl_generate_session_id(s, ss)) {
                    SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_FINAL_SERVER_NAME,
                             ERR_R_INTERNAL_ERROR);
                    return 0;
                }
            } else {
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_FINAL_SERVER_NAME,
                         ERR_R_INTERNAL_ERROR);
                return 0;
            }
        }
    }

    switch (ret) {
    case SSL_TLSEXT_ERR_ALERT_FATAL:
        SSLfatal(s, altmp, SSL_F_FINAL_SERVER_NAME, SSL_R_CALLBACK_FAILED);
        return 0;

    case SSL_TLSEXT_ERR_ALERT_WARNING:
        /* TLSv1.3 doesn't have warning alerts so we suppress this */
        if (!SSL_IS_TLS13(s))
            ssl3_send_alert(s, SSL3_AL_WARNING, altmp);
        s->servername_done = 0;
        return 1;

    case SSL_TLSEXT_ERR_NOACK:
        s->servername_done = 0;
        return 1;

    default:
        return 1;
    }
}