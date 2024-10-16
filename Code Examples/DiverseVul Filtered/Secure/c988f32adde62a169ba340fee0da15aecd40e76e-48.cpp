void ssl_calc_verify_tls( ssl_context *ssl, unsigned char hash[36] )
{
    md5_context md5;
    sha1_context sha1;

    SSL_DEBUG_MSG( 2, ( "=> calc verify tls" ) );

    memcpy( &md5 , &ssl->handshake->fin_md5 , sizeof(md5_context)  );
    memcpy( &sha1, &ssl->handshake->fin_sha1, sizeof(sha1_context) );

     md5_finish( &md5,  hash );
    sha1_finish( &sha1, hash + 16 );

    SSL_DEBUG_BUF( 3, "calculated verify result", hash, 36 );
    SSL_DEBUG_MSG( 2, ( "<= calc verify" ) );

    md5_free(  &md5  );
    sha1_free( &sha1 );

    return;
}