static void ssl_calc_finished_tls_sha384(
                ssl_context *ssl, unsigned char *buf, int from )
{
    int len = 12;
    char *sender;
    sha4_context sha4;
    unsigned char padbuf[48];

    ssl_session *session = ssl->session_negotiate;
    if( !session )
        session = ssl->session;

    SSL_DEBUG_MSG( 2, ( "=> calc  finished tls sha384" ) );

    memcpy( &sha4, &ssl->handshake->fin_sha4, sizeof(sha4_context) );

    /*
     * TLSv1.2:
     *   hash = PRF( master, finished_label,
     *               Hash( handshake ) )[0.11]
     */

    SSL_DEBUG_BUF( 4, "finished sha4 state", (unsigned char *)
                   sha4.state, sizeof( sha4.state ) );

    sender = ( from == SSL_IS_CLIENT )
             ? (char *) "client finished"
             : (char *) "server finished";

    sha4_finish( &sha4, padbuf );

    ssl->handshake->tls_prf( session->master, 48, sender,
                             padbuf, 48, buf, len );

    SSL_DEBUG_BUF( 3, "calc finished result", buf, len );

    memset( &sha4, 0, sizeof( sha4_context ) );

    memset(  padbuf, 0, sizeof(  padbuf ) );

    SSL_DEBUG_MSG( 2, ( "<= calc  finished" ) );
}