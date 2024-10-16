static void evp_cipher_init(struct ssh_cipher_struct *cipher) {
    if (cipher->ctx == NULL) {
        cipher->ctx = EVP_CIPHER_CTX_new();
    }

    switch(cipher->ciphertype){
    case SSH_AES128_CBC:
        cipher->cipher = EVP_aes_128_cbc();
        break;
    case SSH_AES192_CBC:
        cipher->cipher = EVP_aes_192_cbc();
        break;
    case SSH_AES256_CBC:
        cipher->cipher = EVP_aes_256_cbc();
        break;
#ifdef HAVE_OPENSSL_EVP_AES_CTR
    case SSH_AES128_CTR:
        cipher->cipher = EVP_aes_128_ctr();
        break;
    case SSH_AES192_CTR:
        cipher->cipher = EVP_aes_192_ctr();
        break;
    case SSH_AES256_CTR:
        cipher->cipher = EVP_aes_256_ctr();
        break;
#else
    case SSH_AES128_CTR:
    case SSH_AES192_CTR:
    case SSH_AES256_CTR:
        SSH_LOG(SSH_LOG_WARNING, "This cipher is not available in evp_cipher_init");
        break;
#endif
#ifdef HAVE_OPENSSL_EVP_AES_GCM
    case SSH_AEAD_AES128_GCM:
        cipher->cipher = EVP_aes_128_gcm();
        break;
    case SSH_AEAD_AES256_GCM:
        cipher->cipher = EVP_aes_256_gcm();
        break;
#else
    case SSH_AEAD_AES128_GCM:
    case SSH_AEAD_AES256_GCM:
        SSH_LOG(SSH_LOG_WARNING, "This cipher is not available in evp_cipher_init");
        break;
#endif /* HAVE_OPENSSL_EVP_AES_GCM */
    case SSH_3DES_CBC:
        cipher->cipher = EVP_des_ede3_cbc();
        break;
#ifdef WITH_BLOWFISH_CIPHER
    case SSH_BLOWFISH_CBC:
        cipher->cipher = EVP_bf_cbc();
        break;
        /* ciphers not using EVP */
#endif
    case SSH_AEAD_CHACHA20_POLY1305:
        SSH_LOG(SSH_LOG_WARNING, "The ChaCha cipher cannot be handled here");
        break;
    case SSH_NO_CIPHER:
        SSH_LOG(SSH_LOG_WARNING, "No valid ciphertype found");
        break;
    }
}