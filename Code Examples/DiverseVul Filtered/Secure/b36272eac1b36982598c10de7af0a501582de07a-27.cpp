void ssh_crypto_finalize(void)
{
    if (!libcrypto_initialized) {
        return;
    }

    ENGINE_cleanup();
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
#endif

    libcrypto_initialized = 0;
}