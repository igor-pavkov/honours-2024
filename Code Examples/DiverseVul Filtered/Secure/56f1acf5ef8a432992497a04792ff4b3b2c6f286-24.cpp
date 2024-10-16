int SSL_set_generate_session_id(SSL *ssl, GEN_SESSION_CB cb)
{
    CRYPTO_w_lock(CRYPTO_LOCK_SSL);
    ssl->generate_session_id = cb;
    CRYPTO_w_unlock(CRYPTO_LOCK_SSL);
    return 1;
}