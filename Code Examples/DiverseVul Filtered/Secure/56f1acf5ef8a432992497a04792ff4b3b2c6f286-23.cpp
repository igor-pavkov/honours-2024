void ssl_update_cache(SSL *s, int mode)
{
    int i;

    /*
     * If the session_id_length is 0, we are not supposed to cache it, and it
     * would be rather hard to do anyway :-)
     */
    if (s->session->session_id_length == 0)
        return;

    i = s->session_ctx->session_cache_mode;
    if ((i & mode) && (!s->hit)
        && ((i & SSL_SESS_CACHE_NO_INTERNAL_STORE)
            || SSL_CTX_add_session(s->session_ctx, s->session))
        && (s->session_ctx->new_session_cb != NULL)) {
        CRYPTO_add(&s->session->references, 1, CRYPTO_LOCK_SSL_SESSION);
        if (!s->session_ctx->new_session_cb(s, s->session))
            SSL_SESSION_free(s->session);
    }

    /* auto flush every 255 connections */
    if ((!(i & SSL_SESS_CACHE_NO_AUTO_CLEAR)) && ((i & mode) == mode)) {
        if ((((mode & SSL_SESS_CACHE_CLIENT)
              ? s->session_ctx->stats.sess_connect_good
              : s->session_ctx->stats.sess_accept_good) & 0xff) == 0xff) {
            SSL_CTX_flush_sessions(s->session_ctx, (unsigned long)time(NULL));
        }
    }
}