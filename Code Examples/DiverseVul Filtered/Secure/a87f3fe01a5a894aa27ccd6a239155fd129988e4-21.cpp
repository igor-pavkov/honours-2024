int ssl_hmac_final(SSL_HMAC *ctx, unsigned char *md, size_t *len,
                   size_t max_size)
{
    if (ctx->ctx != NULL)
        return EVP_MAC_final(ctx->ctx, md, len, max_size);
#ifndef OPENSSL_NO_DEPRECATED_3_0
    if (ctx->old_ctx != NULL) {
        unsigned int l;

        if (HMAC_Final(ctx->old_ctx, md, &l) > 0) {
            if (len != NULL)
                *len = l;
            return 1;
        }
    }
#endif
    return 0;
}