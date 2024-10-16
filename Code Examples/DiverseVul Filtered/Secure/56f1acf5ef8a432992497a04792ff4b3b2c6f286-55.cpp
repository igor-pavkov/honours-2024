STACK_OF(SSL_CIPHER) *SSL_get_ciphers(const SSL *s)
{
    if (s != NULL) {
        if (s->cipher_list != NULL) {
            return (s->cipher_list);
        } else if ((s->ctx != NULL) && (s->ctx->cipher_list != NULL)) {
            return (s->ctx->cipher_list);
        }
    }
    return (NULL);
}