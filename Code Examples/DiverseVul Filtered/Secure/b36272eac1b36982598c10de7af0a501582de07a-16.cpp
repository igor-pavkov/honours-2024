static void evp_cipher_cleanup(struct ssh_cipher_struct *cipher) {
    if (cipher->ctx != NULL) {
        EVP_CIPHER_CTX_cleanup(cipher->ctx);
        EVP_CIPHER_CTX_free(cipher->ctx);
    }
}