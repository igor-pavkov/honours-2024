int EVP_CIPHER_CTX_reset(EVP_CIPHER_CTX *c)
{
    if (c == NULL)
        return 1;
    if (c->cipher != NULL) {
        if (c->cipher->cleanup && !c->cipher->cleanup(c))
            return 0;
        /* Cleanse cipher context data */
        if (c->cipher_data && c->cipher->ctx_size)
            OPENSSL_cleanse(c->cipher_data, c->cipher->ctx_size);
    }
    OPENSSL_free(c->cipher_data);
#ifndef OPENSSL_NO_ENGINE
    ENGINE_finish(c->engine);
#endif
    memset(c, 0, sizeof(*c));
    return 1;
}