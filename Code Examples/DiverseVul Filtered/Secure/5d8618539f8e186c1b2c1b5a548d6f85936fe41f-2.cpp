int x509_sig_alg_gets( char *buf, size_t size, const x509_buf *sig_oid,
                       pk_type_t pk_alg, md_type_t md_alg,
                       const void *sig_opts )
{
    int ret;
    char *p = buf;
    size_t n = size;
    const char *desc = NULL;

    ret = oid_get_sig_alg_desc( sig_oid, &desc );
    if( ret != 0 )
        ret = snprintf( p, n, "???"  );
    else
        ret = snprintf( p, n, "%s", desc );
    SAFE_SNPRINTF();

#if defined(POLARSSL_X509_RSASSA_PSS_SUPPORT)
    if( pk_alg == POLARSSL_PK_RSASSA_PSS )
    {
        const pk_rsassa_pss_options *pss_opts;
        const md_info_t *md_info, *mgf_md_info;

        pss_opts = (const pk_rsassa_pss_options *) sig_opts;

        md_info = md_info_from_type( md_alg );
        mgf_md_info = md_info_from_type( pss_opts->mgf1_hash_id );

        ret = snprintf( p, n, " (%s, MGF1-%s, 0x%02X)",
                              md_info ? md_info->name : "???",
                              mgf_md_info ? mgf_md_info->name : "???",
                              pss_opts->expected_salt_len );
        SAFE_SNPRINTF();
    }
#else
    ((void) pk_alg);
    ((void) md_alg);
    ((void) sig_opts);
#endif /* POLARSSL_X509_RSASSA_PSS_SUPPORT */

    return( (int)( size - n ) );
}