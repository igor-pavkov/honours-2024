void ssl_clear_cipher_ctx(SSL *s)
{
    if (s->enc_read_ctx != NULL) {
        EVP_CIPHER_CTX_cleanup(s->enc_read_ctx);
        OPENSSL_free(s->enc_read_ctx);
        s->enc_read_ctx = NULL;
    }
    if (s->enc_write_ctx != NULL) {
        EVP_CIPHER_CTX_cleanup(s->enc_write_ctx);
        OPENSSL_free(s->enc_write_ctx);
        s->enc_write_ctx = NULL;
    }
#ifndef OPENSSL_NO_COMP
    if (s->expand != NULL) {
        COMP_CTX_free(s->expand);
        s->expand = NULL;
    }
    if (s->compress != NULL) {
        COMP_CTX_free(s->compress);
        s->compress = NULL;
    }
#endif
}