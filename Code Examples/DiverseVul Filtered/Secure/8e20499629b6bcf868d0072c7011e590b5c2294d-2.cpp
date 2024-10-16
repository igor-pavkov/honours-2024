static int rc4_hmac_md5_init_key(EVP_CIPHER_CTX *ctx,
                                 const unsigned char *inkey,
                                 const unsigned char *iv, int enc)
{
    EVP_RC4_HMAC_MD5 *key = data(ctx);

    RC4_set_key(&key->ks, EVP_CIPHER_CTX_key_length(ctx), inkey);

    MD5_Init(&key->head);       /* handy when benchmarking */
    key->tail = key->head;
    key->md = key->head;

    key->payload_length = NO_PAYLOAD_LENGTH;

    return 1;
}