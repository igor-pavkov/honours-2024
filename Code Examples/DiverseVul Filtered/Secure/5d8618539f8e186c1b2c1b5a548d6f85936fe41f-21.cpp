int x509_get_serial( unsigned char **p, const unsigned char *end,
                     x509_buf *serial )
{
    int ret;

    if( ( end - *p ) < 1 )
        return( POLARSSL_ERR_X509_INVALID_SERIAL +
                POLARSSL_ERR_ASN1_OUT_OF_DATA );

    if( **p != ( ASN1_CONTEXT_SPECIFIC | ASN1_PRIMITIVE | 2 ) &&
        **p !=   ASN1_INTEGER )
        return( POLARSSL_ERR_X509_INVALID_SERIAL +
                POLARSSL_ERR_ASN1_UNEXPECTED_TAG );

    serial->tag = *(*p)++;

    if( ( ret = asn1_get_len( p, end, &serial->len ) ) != 0 )
        return( POLARSSL_ERR_X509_INVALID_SERIAL + ret );

    serial->p = *p;
    *p += serial->len;

    return( 0 );
}