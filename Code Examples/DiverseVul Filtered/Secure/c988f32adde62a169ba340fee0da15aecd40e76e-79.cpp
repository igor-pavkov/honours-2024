static void ssl_calc_finished_tls_sha384(
                ssl_context *ssl, unsigned char *buf, int from )
{
    int len = 12;
    const char *sender;
    sha512_context sha512;
    unsigned char padbuf[48];

    ssl_session *session = ssl->session_negotiate;
    if( !session )
        session = ssl->session;

    SSL_DEBUG_MSG( 2, ( "=> calc  finished tls sha384" ) );

    memcpy( &sha512, &ssl->handshake->fin_sha512, sizeof(sha512_context) );

    /*
     * TLSv1.2:
     *   hash = PRF( master, finished_label,
     *               Hash( handshake ) )[0.11]
     */

#if !defined(POLARSSL_SHA512_ALT)
    SSL_DEBUG_BUF( 4, "finished sha512 state", (unsigned char *)
                   sha512.state, sizeof( sha512.state ) );
#endif

    sender = ( from == SSL_IS_CLIENT )
             ? "client finished"
             : "server finished";

    sha512_finish( &sha512, padbuf );

    ssl->handshake->tls_prf( session->master, 48, sender,
                             padbuf, 48, buf, len );

    SSL_DEBUG_BUF( 3, "calc finished result", buf, len );

    sha512_free( &sha512 );

    polarssl_zeroize(  padbuf, sizeof( padbuf ) );

    SSL_DEBUG_MSG( 2, ( "<= calc  finished" ) );
}