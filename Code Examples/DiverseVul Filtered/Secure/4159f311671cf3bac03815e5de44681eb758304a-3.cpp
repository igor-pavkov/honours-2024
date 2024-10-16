static int aesni_cbc_hmac_sha1_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg,
                                    void *ptr)
{
    EVP_AES_HMAC_SHA1 *key = data(ctx);

    switch (type) {
    case EVP_CTRL_AEAD_SET_MAC_KEY:
        {
            unsigned int i;
            unsigned char hmac_key[64];

            memset(hmac_key, 0, sizeof(hmac_key));

            if (arg > (int)sizeof(hmac_key)) {
                SHA1_Init(&key->head);
                SHA1_Update(&key->head, ptr, arg);
                SHA1_Final(hmac_key, &key->head);
            } else {
                memcpy(hmac_key, ptr, arg);
            }

            for (i = 0; i < sizeof(hmac_key); i++)
                hmac_key[i] ^= 0x36; /* ipad */
            SHA1_Init(&key->head);
            SHA1_Update(&key->head, hmac_key, sizeof(hmac_key));

            for (i = 0; i < sizeof(hmac_key); i++)
                hmac_key[i] ^= 0x36 ^ 0x5c; /* opad */
            SHA1_Init(&key->tail);
            SHA1_Update(&key->tail, hmac_key, sizeof(hmac_key));

            OPENSSL_cleanse(hmac_key, sizeof(hmac_key));

            return 1;
        }
    case EVP_CTRL_AEAD_TLS1_AAD:
        {
            unsigned char *p = ptr;
            unsigned int len;

            if (arg != EVP_AEAD_TLS1_AAD_LEN)
                return -1;
 
            len = p[arg - 2] << 8 | p[arg - 1];

            if (ctx->encrypt) {
                key->payload_length = len;
                if ((key->aux.tls_ver =
                     p[arg - 4] << 8 | p[arg - 3]) >= TLS1_1_VERSION) {
                    len -= AES_BLOCK_SIZE;
                    p[arg - 2] = len >> 8;
                    p[arg - 1] = len;
                }
                key->md = key->head;
                SHA1_Update(&key->md, p, arg);

                return (int)(((len + SHA_DIGEST_LENGTH +
                               AES_BLOCK_SIZE) & -AES_BLOCK_SIZE)
                             - len);
            } else {
                memcpy(key->aux.tls_aad, ptr, arg);
                key->payload_length = arg;

                return SHA_DIGEST_LENGTH;
            }
        }
    default:
        return -1;
    }
}