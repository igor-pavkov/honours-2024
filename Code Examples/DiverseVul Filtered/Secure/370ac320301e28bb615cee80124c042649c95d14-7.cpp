int X509_STORE_CTX_get_ex_new_index(long argl, void *argp,
                                    CRYPTO_EX_new *new_func,
                                    CRYPTO_EX_dup *dup_func,
                                    CRYPTO_EX_free *free_func)
{
    /*
     * This function is (usually) called only once, by
     * SSL_get_ex_data_X509_STORE_CTX_idx (ssl/ssl_cert.c).
     */
    return CRYPTO_get_ex_new_index(CRYPTO_EX_INDEX_X509_STORE_CTX, argl, argp,
                                   new_func, dup_func, free_func);
}