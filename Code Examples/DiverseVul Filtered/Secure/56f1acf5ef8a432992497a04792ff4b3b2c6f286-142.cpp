STACK_OF(SSL_CIPHER) *ssl_get_ciphers_by_id(SSL *s)
{
    if (s != NULL) {
        if (s->cipher_list_by_id != NULL) {
            return (s->cipher_list_by_id);
        } else if ((s->ctx != NULL) && (s->ctx->cipher_list_by_id != NULL)) {
            return (s->ctx->cipher_list_by_id);
        }
    }
    return (NULL);
}