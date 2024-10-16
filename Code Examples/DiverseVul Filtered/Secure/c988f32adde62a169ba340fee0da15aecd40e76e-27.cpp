void ssl_set_ciphersuites_for_version( ssl_context *ssl,
                                       const int *ciphersuites,
                                       int major, int minor )
{
    if( major != SSL_MAJOR_VERSION_3 )
        return;

    if( minor < SSL_MINOR_VERSION_0 || minor > SSL_MINOR_VERSION_3 )
        return;

    ssl->ciphersuite_list[minor] = ciphersuites;
}