void ssl_calc_verify_tls_sha256( ssl_context *ssl, unsigned char hash[32] )
{
    sha256_context sha256;

    SSL_DEBUG_MSG( 2, ( "=> calc verify sha256" ) );

    memcpy( &sha256, &ssl->handshake->fin_sha256, sizeof(sha256_context) );
    sha256_finish( &sha256, hash );

    SSL_DEBUG_BUF( 3, "calculated verify result", hash, 32 );
    SSL_DEBUG_MSG( 2, ( "<= calc verify" ) );

    sha256_free( &sha256 );

    return;
}