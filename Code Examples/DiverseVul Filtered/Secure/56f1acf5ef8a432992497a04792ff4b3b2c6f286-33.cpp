int SSL_CTX_set_generate_session_id(SSL_CTX *ctx, GEN_SESSION_CB cb)
{
    CRYPTO_w_lock(CRYPTO_LOCK_SSL_CTX);
    ctx->generate_session_id = cb;
    CRYPTO_w_unlock(CRYPTO_LOCK_SSL_CTX);
    return 1;
}