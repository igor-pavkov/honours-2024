EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void)
{
    return OPENSSL_zalloc(sizeof(EVP_CIPHER_CTX));
}