static int aes_ctr_set_key(struct ssh_cipher_struct *cipher, void *key,
    void *IV) {
    int rc;

    if (cipher->aes_key == NULL) {
        cipher->aes_key = malloc(sizeof (struct ssh_aes_key_schedule));
    }
    if (cipher->aes_key == NULL) {
        return SSH_ERROR;
    }
    ZERO_STRUCTP(cipher->aes_key);
    /* CTR doesn't need a decryption key */
    rc = AES_set_encrypt_key(key, cipher->keysize, &cipher->aes_key->key);
    if (rc < 0) {
        SAFE_FREE(cipher->aes_key);
        return SSH_ERROR;
    }
    memcpy(cipher->aes_key->IV, IV, AES_BLOCK_SIZE);
    return SSH_OK;
}