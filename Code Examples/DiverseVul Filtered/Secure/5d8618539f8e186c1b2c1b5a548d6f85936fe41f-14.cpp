int x509_self_test( int verbose )
{
#if defined(POLARSSL_CERTS_C) && defined(POLARSSL_SHA1_C)
    int ret;
    int flags;
    x509_crt cacert;
    x509_crt clicert;

    if( verbose != 0 )
        polarssl_printf( "  X.509 certificate load: " );

    x509_crt_init( &clicert );

    ret = x509_crt_parse( &clicert, (const unsigned char *) test_cli_crt,
                          strlen( test_cli_crt ) );
    if( ret != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( ret );
    }

    x509_crt_init( &cacert );

    ret = x509_crt_parse( &cacert, (const unsigned char *) test_ca_crt,
                          strlen( test_ca_crt ) );
    if( ret != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        return( ret );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n  X.509 signature verify: ");

    ret = x509_crt_verify( &clicert, &cacert, NULL, NULL, &flags, NULL, NULL );
    if( ret != 0 )
    {
        if( verbose != 0 )
            polarssl_printf( "failed\n" );

        polarssl_printf( "ret = %d, &flags = %04x\n", ret, flags );

        return( ret );
    }

    if( verbose != 0 )
        polarssl_printf( "passed\n\n");

    x509_crt_free( &cacert  );
    x509_crt_free( &clicert );

    return( 0 );
#else
    ((void) verbose);
    return( POLARSSL_ERR_X509_FEATURE_UNAVAILABLE );
#endif /* POLARSSL_CERTS_C && POLARSSL_SHA1_C */
}