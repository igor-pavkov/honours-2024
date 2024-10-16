void ssl_calc_verify_tls_sha384( ssl_context *ssl, unsigned char hash[48] )
{
    sha512_context sha512;

    SSL_DEBUG_MSG( 2, ( "=> calc verify sha384" ) );

    memcpy( &sha512, &ssl->handshake->fin_sha512, sizeof(sha512_context) );
    sha512_finish( &sha512, hash );

    SSL_DEBUG_BUF( 3, "calculated verify result", hash, 48 );
    SSL_DEBUG_MSG( 2, ( "<= calc verify" ) );

    sha512_free( &sha512 );

    return;
}