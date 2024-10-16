int x509_get_sig( unsigned char **p, const unsigned char *end, x509_buf *sig )
{
    int ret;
    size_t len;

    if( ( end - *p ) < 1 )
        return( POLARSSL_ERR_X509_INVALID_SIGNATURE +
                POLARSSL_ERR_ASN1_OUT_OF_DATA );

    sig->tag = **p;

    if( ( ret = asn1_get_bitstring_null( p, end, &len ) ) != 0 )
        return( POLARSSL_ERR_X509_INVALID_SIGNATURE + ret );

    sig->len = len;
    sig->p = *p;

    *p += len;

    return( 0 );
}