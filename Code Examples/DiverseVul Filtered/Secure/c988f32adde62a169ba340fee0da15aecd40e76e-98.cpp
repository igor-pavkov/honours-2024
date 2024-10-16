static void ssl_calc_finished_tls_sha256(
                ssl_context *ssl, unsigned char *buf, int from )
{
    int len = 12;
    const char *sender;
    sha256_context sha256;
    unsigned char padbuf[32];

    ssl_session *session = ssl->session_negotiate;
    if( !session )
        session = ssl->session;

    SSL_DEBUG_MSG( 2, ( "=> calc  finished tls sha256" ) );

    memcpy( &sha256, &ssl->handshake->fin_sha256, sizeof(sha256_context) );

    /*
     * TLSv1.2:
     *   hash = PRF( master, finished_label,
     *               Hash( handshake ) )[0.11]
     */

#if !defined(POLARSSL_SHA256_ALT)
    SSL_DEBUG_BUF( 4, "finished sha2 state", (unsigned char *)
                   sha256.state, sizeof( sha256.state ) );
#endif

    sender = ( from == SSL_IS_CLIENT )
             ? "client finished"
             : "server finished";

    sha256_finish( &sha256, padbuf );

    ssl->handshake->tls_prf( session->master, 48, sender,
                             padbuf, 32, buf, len );

    SSL_DEBUG_BUF( 3, "calc finished result", buf, len );

    sha256_free( &sha256 );

    polarssl_zeroize(  padbuf, sizeof(  padbuf ) );

    SSL_DEBUG_MSG( 2, ( "<= calc  finished" ) );
}