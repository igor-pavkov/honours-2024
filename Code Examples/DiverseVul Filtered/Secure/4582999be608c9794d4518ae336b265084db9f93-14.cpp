void ssl_calc_verify_tls_sha256( ssl_context *ssl, unsigned char hash[32] )
{
    sha2_context sha2;

    SSL_DEBUG_MSG( 2, ( "=> calc verify sha256" ) );

    memcpy( &sha2, &ssl->handshake->fin_sha2, sizeof(sha2_context) );
    sha2_finish( &sha2, hash );

    SSL_DEBUG_BUF( 3, "calculated verify result", hash, 32 );
    SSL_DEBUG_MSG( 2, ( "<= calc verify" ) );

    return;
}