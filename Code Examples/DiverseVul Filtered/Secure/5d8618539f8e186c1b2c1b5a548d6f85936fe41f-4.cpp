int x509_get_sig_alg( const x509_buf *sig_oid, const x509_buf *sig_params,
                      md_type_t *md_alg, pk_type_t *pk_alg,
                      void **sig_opts )
{
    int ret;

    if( *sig_opts != NULL )
        return( POLARSSL_ERR_X509_BAD_INPUT_DATA );

    if( ( ret = oid_get_sig_alg( sig_oid, md_alg, pk_alg ) ) != 0 )
        return( POLARSSL_ERR_X509_UNKNOWN_SIG_ALG + ret );

#if defined(POLARSSL_X509_RSASSA_PSS_SUPPORT)
    if( *pk_alg == POLARSSL_PK_RSASSA_PSS )
    {
        pk_rsassa_pss_options *pss_opts;

        pss_opts = polarssl_malloc( sizeof( pk_rsassa_pss_options ) );
        if( pss_opts == NULL )
            return( POLARSSL_ERR_X509_MALLOC_FAILED );

        ret = x509_get_rsassa_pss_params( sig_params,
                                          md_alg,
                                          &pss_opts->mgf1_hash_id,
                                          &pss_opts->expected_salt_len );
        if( ret != 0 )
        {
            polarssl_free( pss_opts );
            return( ret );
        }

        *sig_opts = (void *) pss_opts;
    }
    else
#endif /* POLARSSL_X509_RSASSA_PSS_SUPPORT */
    {
        /* Make sure parameters are absent or NULL */
        if( ( sig_params->tag != ASN1_NULL && sig_params->tag != 0 ) ||
              sig_params->len != 0 )
        return( POLARSSL_ERR_X509_INVALID_ALG );
    }

    return( 0 );
}