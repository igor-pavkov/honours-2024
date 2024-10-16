int SSL_has_matching_session_id(const SSL *ssl, const unsigned char *id,
                                unsigned int id_len)
{
    /*
     * A quick examination of SSL_SESSION_hash and SSL_SESSION_cmp shows how
     * we can "construct" a session to give us the desired check - ie. to
     * find if there's a session in the hash table that would conflict with
     * any new session built out of this id/id_len and the ssl_version in use
     * by this SSL.
     */
    SSL_SESSION r, *p;

    if (id_len > sizeof r.session_id)
        return 0;

    r.ssl_version = ssl->version;
    r.session_id_length = id_len;
    memcpy(r.session_id, id, id_len);
    /*
     * NB: SSLv2 always uses a fixed 16-byte session ID, so even if a
     * callback is calling us to check the uniqueness of a shorter ID, it
     * must be compared as a padded-out ID because that is what it will be
     * converted to when the callback has finished choosing it.
     */
    if ((r.ssl_version == SSL2_VERSION) &&
        (id_len < SSL2_SSL_SESSION_ID_LENGTH)) {
        memset(r.session_id + id_len, 0, SSL2_SSL_SESSION_ID_LENGTH - id_len);
        r.session_id_length = SSL2_SSL_SESSION_ID_LENGTH;
    }

    CRYPTO_r_lock(CRYPTO_LOCK_SSL_CTX);
    p = lh_SSL_SESSION_retrieve(ssl->ctx->sessions, &r);
    CRYPTO_r_unlock(CRYPTO_LOCK_SSL_CTX);
    return (p != NULL);
}