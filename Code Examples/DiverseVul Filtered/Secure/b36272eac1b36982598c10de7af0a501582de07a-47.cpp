int ssh_crypto_init(void)
{
    UNUSED_VAR(size_t i);

    if (libcrypto_initialized) {
        return SSH_OK;
    }
    if (SSLeay() != OPENSSL_VERSION_NUMBER){
        SSH_LOG(SSH_LOG_WARNING, "libssh compiled with %s "
            "headers, currently running with %s.",
            OPENSSL_VERSION_TEXT,
            SSLeay_version(SSLeay())
        );
    }
#ifdef CAN_DISABLE_AESNI
    /*
     * disable AES-NI when running within Valgrind, because they generate
     * too many "uninitialized memory access" false positives
     */
    if (RUNNING_ON_VALGRIND){
        SSH_LOG(SSH_LOG_INFO, "Running within Valgrind, disabling AES-NI");
        /* Bit #57 denotes AES-NI instruction set extension */
        OPENSSL_ia32cap &= ~(1LL << 57);
    }
#endif
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    OpenSSL_add_all_algorithms();
#endif

#if !defined(HAVE_OPENSSL_EVP_CHACHA20) || !defined(HAVE_OPENSSL_EVP_POLY1305)
    for (i = 0; ssh_ciphertab[i].name != NULL; i++) {
        int cmp;

        cmp = strcmp(ssh_ciphertab[i].name, "chacha20-poly1305@openssh.com");
        if (cmp == 0) {
            memcpy(&ssh_ciphertab[i],
                   ssh_get_chacha20poly1305_cipher(),
                   sizeof(struct ssh_cipher_struct));
            break;
        }
    }
#endif /* !defined(HAVE_OPENSSL_EVP_CHACHA20) || !defined(HAVE_OPENSSL_EVP_POLY1305) */

    libcrypto_initialized = 1;

    return SSH_OK;
}