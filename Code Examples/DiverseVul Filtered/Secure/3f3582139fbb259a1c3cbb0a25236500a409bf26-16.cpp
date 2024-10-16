int EVP_CIPHER_CTX_copy(EVP_CIPHER_CTX *out, const EVP_CIPHER_CTX *in)
{
    if ((in == NULL) || (in->cipher == NULL)) {
        EVPerr(EVP_F_EVP_CIPHER_CTX_COPY, EVP_R_INPUT_NOT_INITIALIZED);
        return 0;
    }
#ifndef OPENSSL_NO_ENGINE
    /* Make sure it's safe to copy a cipher context using an ENGINE */
    if (in->engine && !ENGINE_init(in->engine)) {
        EVPerr(EVP_F_EVP_CIPHER_CTX_COPY, ERR_R_ENGINE_LIB);
        return 0;
    }
#endif

    EVP_CIPHER_CTX_reset(out);
    memcpy(out, in, sizeof(*out));

    if (in->cipher_data && in->cipher->ctx_size) {
        out->cipher_data = OPENSSL_malloc(in->cipher->ctx_size);
        if (out->cipher_data == NULL) {
            EVPerr(EVP_F_EVP_CIPHER_CTX_COPY, ERR_R_MALLOC_FAILURE);
            return 0;
        }
        memcpy(out->cipher_data, in->cipher_data, in->cipher->ctx_size);
    }

    if (in->cipher->flags & EVP_CIPH_CUSTOM_COPY)
        return in->cipher->ctrl((EVP_CIPHER_CTX *)in, EVP_CTRL_COPY, 0, out);
    return 1;
}