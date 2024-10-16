const char *ssl_get_version( const ssl_context *ssl )
{
    switch( ssl->minor_ver )
    {
        case SSL_MINOR_VERSION_0:
            return( "SSLv3.0" );

        case SSL_MINOR_VERSION_1:
            return( "TLSv1.0" );

        case SSL_MINOR_VERSION_2:
            return( "TLSv1.1" );

        case SSL_MINOR_VERSION_3:
            return( "TLSv1.2" );

        default:
            break;
    }
    return( "unknown" );
}