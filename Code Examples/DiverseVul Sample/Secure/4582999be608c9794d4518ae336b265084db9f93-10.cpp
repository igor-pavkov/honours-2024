static void ssl_calc_finished_tls_sha256(
                ssl_context *ssl, unsigned char *buf, int from )
{
    int len = 12;
    char *sender;
    sha2_context sha2;
    unsigned char padbuf[32];

    ssl_session *session = ssl->session_negotiate;
    if( !session )
        session = ssl->session;

    SSL_DEBUG_MSG( 2, ( "=> calc  finished tls sha256" ) );

    memcpy( &sha2, &ssl->handshake->fin_sha2, sizeof(sha2_context) );

    /*
     * TLSv1.2:
     *   hash = PRF( master, finished_label,
     *               Hash( handshake ) )[0.11]
     */

    SSL_DEBUG_BUF( 4, "finished sha2 state", (unsigned char *)
                   sha2.state, sizeof( sha2.state ) );

    sender = ( from == SSL_IS_CLIENT )
             ? (char *) "client finished"
             : (char *) "server finished";

    sha2_finish( &sha2, padbuf );

    ssl->handshake->tls_prf( session->master, 48, sender,
                             padbuf, 32, buf, len );

    SSL_DEBUG_BUF( 3, "calc finished result", buf, len );

    memset( &sha2, 0, sizeof( sha2_context ) );

    memset(  padbuf, 0, sizeof(  padbuf ) );

    SSL_DEBUG_MSG( 2, ( "<= calc  finished" ) );
}