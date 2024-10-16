void ssl_calc_verify_ssl( ssl_context *ssl, unsigned char hash[36] )
{
    md5_context md5;
    sha1_context sha1;
    unsigned char pad_1[48];
    unsigned char pad_2[48];

    SSL_DEBUG_MSG( 2, ( "=> calc verify ssl" ) );

    memcpy( &md5 , &ssl->handshake->fin_md5 , sizeof(md5_context)  );
    memcpy( &sha1, &ssl->handshake->fin_sha1, sizeof(sha1_context) );

    memset( pad_1, 0x36, 48 );
    memset( pad_2, 0x5C, 48 );

    md5_update( &md5, ssl->session_negotiate->master, 48 );
    md5_update( &md5, pad_1, 48 );
    md5_finish( &md5, hash );

    md5_starts( &md5 );
    md5_update( &md5, ssl->session_negotiate->master, 48 );
    md5_update( &md5, pad_2, 48 );
    md5_update( &md5, hash,  16 );
    md5_finish( &md5, hash );

    sha1_update( &sha1, ssl->session_negotiate->master, 48 );
    sha1_update( &sha1, pad_1, 40 );
    sha1_finish( &sha1, hash + 16 );

    sha1_starts( &sha1 );
    sha1_update( &sha1, ssl->session_negotiate->master, 48 );
    sha1_update( &sha1, pad_2, 40 );
    sha1_update( &sha1, hash + 16, 20 );
    sha1_finish( &sha1, hash + 16 );

    SSL_DEBUG_BUF( 3, "calculated verify result", hash, 36 );
    SSL_DEBUG_MSG( 2, ( "<= calc verify" ) );

    md5_free(  &md5  );
    sha1_free( &sha1 );

    return;
}