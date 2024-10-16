static int evp_cipher_set_encrypt_key(struct ssh_cipher_struct *cipher,
            void *key, void *IV)
{
    int rc;

    evp_cipher_init(cipher);
    EVP_CIPHER_CTX_init(cipher->ctx);

    rc = EVP_EncryptInit_ex(cipher->ctx, cipher->cipher, NULL, key, IV);
    if (rc != 1){
        SSH_LOG(SSH_LOG_WARNING, "EVP_EncryptInit_ex failed");
        return SSH_ERROR;
    }

#ifdef HAVE_OPENSSL_EVP_AES_GCM
    /* For AES-GCM we need to set IV in specific way */
    if (cipher->ciphertype == SSH_AEAD_AES128_GCM ||
        cipher->ciphertype == SSH_AEAD_AES256_GCM) {
        rc = EVP_CIPHER_CTX_ctrl(cipher->ctx,
                                 EVP_CTRL_GCM_SET_IV_FIXED,
                                 -1,
                                 (uint8_t *)IV);
        if (rc != 1) {
            SSH_LOG(SSH_LOG_WARNING, "EVP_CTRL_GCM_SET_IV_FIXED failed");
            return SSH_ERROR;
        }
    }
#endif /* HAVE_OPENSSL_EVP_AES_GCM */

    EVP_CIPHER_CTX_set_padding(cipher->ctx, 0);

    return SSH_OK;
}