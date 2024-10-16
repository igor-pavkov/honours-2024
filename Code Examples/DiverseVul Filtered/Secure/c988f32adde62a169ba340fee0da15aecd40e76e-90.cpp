void ssl_set_ciphersuites( ssl_context *ssl, const int *ciphersuites )
{
    ssl->ciphersuite_list[SSL_MINOR_VERSION_0] = ciphersuites;
    ssl->ciphersuite_list[SSL_MINOR_VERSION_1] = ciphersuites;
    ssl->ciphersuite_list[SSL_MINOR_VERSION_2] = ciphersuites;
    ssl->ciphersuite_list[SSL_MINOR_VERSION_3] = ciphersuites;
}