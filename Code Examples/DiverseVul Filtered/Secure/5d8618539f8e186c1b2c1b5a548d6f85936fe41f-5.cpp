int x509_get_alg( unsigned char **p, const unsigned char *end,
                  x509_buf *alg, x509_buf *params )
{
    int ret;

    if( ( ret = asn1_get_alg( p, end, alg, params ) ) != 0 )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    return( 0 );
}