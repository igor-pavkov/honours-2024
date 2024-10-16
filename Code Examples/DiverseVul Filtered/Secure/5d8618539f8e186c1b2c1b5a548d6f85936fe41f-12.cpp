static int x509_get_hash_alg( const x509_buf *alg, md_type_t *md_alg )
{
    int ret;
    unsigned char *p;
    const unsigned char *end;
    x509_buf md_oid;
    size_t len;

    /* Make sure we got a SEQUENCE and setup bounds */
    if( alg->tag != ( ASN1_CONSTRUCTED | ASN1_SEQUENCE ) )
        return( POLARSSL_ERR_X509_INVALID_ALG +
                POLARSSL_ERR_ASN1_UNEXPECTED_TAG );

    p = (unsigned char *) alg->p;
    end = p + alg->len;

    if( p >= end )
        return( POLARSSL_ERR_X509_INVALID_ALG +
                POLARSSL_ERR_ASN1_OUT_OF_DATA );

    /* Parse md_oid */
    md_oid.tag = *p;

    if( ( ret = asn1_get_tag( &p, end, &md_oid.len, ASN1_OID ) ) != 0 )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    md_oid.p = p;
    p += md_oid.len;

    /* Get md_alg from md_oid */
    if( ( ret = oid_get_md_alg( &md_oid, md_alg ) ) != 0 )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    /* Make sure params is absent of NULL */
    if( p == end )
        return( 0 );

    if( ( ret = asn1_get_tag( &p, end, &len, ASN1_NULL ) ) != 0 || len != 0 )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    if( p != end )
        return( POLARSSL_ERR_X509_INVALID_ALG +
                POLARSSL_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}