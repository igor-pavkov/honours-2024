void ssl_calc_verify_tls_sha384( ssl_context *ssl, unsigned char hash[48] )
{
    sha4_context sha4;

    SSL_DEBUG_MSG( 2, ( "=> calc verify sha384" ) );

    memcpy( &sha4, &ssl->handshake->fin_sha4, sizeof(sha4_context) );
    sha4_finish( &sha4, hash );

    SSL_DEBUG_BUF( 3, "calculated verify result", hash, 48 );
    SSL_DEBUG_MSG( 2, ( "<= calc verify" ) );

    return;
}