int SSL_CTX_set_ssl_version(SSL_CTX *ctx, const SSL_METHOD *meth)
{
    STACK_OF(SSL_CIPHER) *sk;

    ctx->method = meth;

    sk = ssl_create_cipher_list(ctx->method, &(ctx->cipher_list),
                                &(ctx->cipher_list_by_id),
                                meth->version ==
                                SSL2_VERSION ? "SSLv2" :
                                SSL_DEFAULT_CIPHER_LIST);
    if ((sk == NULL) || (sk_SSL_CIPHER_num(sk) <= 0)) {
        SSLerr(SSL_F_SSL_CTX_SET_SSL_VERSION,
               SSL_R_SSL_LIBRARY_HAS_NO_CIPHERS);
        return (0);
    }
    return (1);
}