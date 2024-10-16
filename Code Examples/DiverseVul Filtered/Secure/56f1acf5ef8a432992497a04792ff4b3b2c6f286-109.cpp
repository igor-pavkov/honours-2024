int SSL_set_cipher_list(SSL *s, const char *str)
{
    STACK_OF(SSL_CIPHER) *sk;

    sk = ssl_create_cipher_list(s->ctx->method, &s->cipher_list,
                                &s->cipher_list_by_id, str);
    /* see comment in SSL_CTX_set_cipher_list */
    if (sk == NULL)
        return 0;
    else if (sk_SSL_CIPHER_num(sk) == 0) {
        SSLerr(SSL_F_SSL_SET_CIPHER_LIST, SSL_R_NO_CIPHER_MATCH);
        return 0;
    }
    return 1;
}