int x509_key_size_helper( char *buf, size_t size, const char *name )
{
    char *p = buf;
    size_t n = size;
    int ret;

    if( strlen( name ) + sizeof( " key size" ) > size )
        return( POLARSSL_ERR_DEBUG_BUF_TOO_SMALL );

    ret = snprintf( p, n, "%s key size", name );
    SAFE_SNPRINTF();

    return( 0 );
}