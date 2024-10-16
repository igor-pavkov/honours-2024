const DSA_METHOD *DSA_OpenSSL(void)
{
    return &openssl_dsa_meth;
}