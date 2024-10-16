int x509_get_alg_null( unsigned char **p, const unsigned char *end,
                       x509_buf *alg )
{
    int ret;

    if( ( ret = asn1_get_alg_null( p, end, alg ) ) != 0 )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    return( 0 );
}