static int aesni_cbc_hmac_sha1_init_key(EVP_CIPHER_CTX *ctx,
                                        const unsigned char *inkey,
                                        const unsigned char *iv, int enc)
{
    EVP_AES_HMAC_SHA1 *key = data(ctx);
    int ret;

    if (enc)
        ret = aesni_set_encrypt_key(inkey, ctx->key_len * 8, &key->ks);
    else
        ret = aesni_set_decrypt_key(inkey, ctx->key_len * 8, &key->ks);

    SHA1_Init(&key->head);      /* handy when benchmarking */
    key->tail = key->head;
    key->md = key->head;

    key->payload_length = NO_PAYLOAD_LENGTH;

    return ret < 0 ? 0 : 1;
}