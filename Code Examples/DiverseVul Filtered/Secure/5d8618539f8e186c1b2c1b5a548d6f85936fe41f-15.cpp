int x509_get_rsassa_pss_params( const x509_buf *params,
                                md_type_t *md_alg, md_type_t *mgf_md,
                                int *salt_len )
{
    int ret;
    unsigned char *p;
    const unsigned char *end, *end2;
    size_t len;
    x509_buf alg_id, alg_params;

    /* First set everything to defaults */
    *md_alg = POLARSSL_MD_SHA1;
    *mgf_md = POLARSSL_MD_SHA1;
    *salt_len = 20;

    /* Make sure params is a SEQUENCE and setup bounds */
    if( params->tag != ( ASN1_CONSTRUCTED | ASN1_SEQUENCE ) )
        return( POLARSSL_ERR_X509_INVALID_ALG +
                POLARSSL_ERR_ASN1_UNEXPECTED_TAG );

    p = (unsigned char *) params->p;
    end = p + params->len;

    if( p == end )
        return( 0 );

    /*
     * HashAlgorithm
     */
    if( ( ret = asn1_get_tag( &p, end, &len,
                    ASN1_CONTEXT_SPECIFIC | ASN1_CONSTRUCTED | 0 ) ) == 0 )
    {
        end2 = p + len;

        /* HashAlgorithm ::= AlgorithmIdentifier (without parameters) */
        if( ( ret = x509_get_alg_null( &p, end2, &alg_id ) ) != 0 )
            return( ret );

        if( ( ret = oid_get_md_alg( &alg_id, md_alg ) ) != 0 )
            return( POLARSSL_ERR_X509_INVALID_ALG + ret );

        if( p != end2 )
            return( POLARSSL_ERR_X509_INVALID_ALG +
                    POLARSSL_ERR_ASN1_LENGTH_MISMATCH );
    }
    else if( ret != POLARSSL_ERR_ASN1_UNEXPECTED_TAG )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    if( p == end )
        return( 0 );

    /*
     * MaskGenAlgorithm
     */
    if( ( ret = asn1_get_tag( &p, end, &len,
                    ASN1_CONTEXT_SPECIFIC | ASN1_CONSTRUCTED | 1 ) ) == 0 )
    {
        end2 = p + len;

        /* MaskGenAlgorithm ::= AlgorithmIdentifier (params = HashAlgorithm) */
        if( ( ret = x509_get_alg( &p, end2, &alg_id, &alg_params ) ) != 0 )
            return( ret );

        /* Only MFG1 is recognised for now */
        if( ! OID_CMP( OID_MGF1, &alg_id ) )
            return( POLARSSL_ERR_X509_FEATURE_UNAVAILABLE +
                    POLARSSL_ERR_OID_NOT_FOUND );

        /* Parse HashAlgorithm */
        if( ( ret = x509_get_hash_alg( &alg_params, mgf_md ) ) != 0 )
            return( ret );

        if( p != end2 )
            return( POLARSSL_ERR_X509_INVALID_ALG +
                    POLARSSL_ERR_ASN1_LENGTH_MISMATCH );
    }
    else if( ret != POLARSSL_ERR_ASN1_UNEXPECTED_TAG )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    if( p == end )
        return( 0 );

    /*
     * salt_len
     */
    if( ( ret = asn1_get_tag( &p, end, &len,
                    ASN1_CONTEXT_SPECIFIC | ASN1_CONSTRUCTED | 2 ) ) == 0 )
    {
        end2 = p + len;

        if( ( ret = asn1_get_int( &p, end2, salt_len ) ) != 0 )
            return( POLARSSL_ERR_X509_INVALID_ALG + ret );

        if( p != end2 )
            return( POLARSSL_ERR_X509_INVALID_ALG +
                    POLARSSL_ERR_ASN1_LENGTH_MISMATCH );
    }
    else if( ret != POLARSSL_ERR_ASN1_UNEXPECTED_TAG )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    if( p == end )
        return( 0 );

    /*
     * trailer_field (if present, must be 1)
     */
    if( ( ret = asn1_get_tag( &p, end, &len,
                    ASN1_CONTEXT_SPECIFIC | ASN1_CONSTRUCTED | 3 ) ) == 0 )
    {
        int trailer_field;

        end2 = p + len;

        if( ( ret = asn1_get_int( &p, end2, &trailer_field ) ) != 0 )
            return( POLARSSL_ERR_X509_INVALID_ALG + ret );

        if( p != end2 )
            return( POLARSSL_ERR_X509_INVALID_ALG +
                    POLARSSL_ERR_ASN1_LENGTH_MISMATCH );

        if( trailer_field != 1 )
            return( POLARSSL_ERR_X509_INVALID_ALG );
    }
    else if( ret != POLARSSL_ERR_ASN1_UNEXPECTED_TAG )
        return( POLARSSL_ERR_X509_INVALID_ALG + ret );

    if( p != end )
        return( POLARSSL_ERR_X509_INVALID_ALG +
                POLARSSL_ERR_ASN1_LENGTH_MISMATCH );

    return( 0 );
}