static void ssl_calc_finished_tls(
                ssl_context *ssl, unsigned char *buf, int from )
{
    int len = 12;
    const char *sender;
    md5_context  md5;
    sha1_context sha1;
    unsigned char padbuf[36];

    ssl_session *session = ssl->session_negotiate;
    if( !session )
        session = ssl->session;

    SSL_DEBUG_MSG( 2, ( "=> calc  finished tls" ) );

    memcpy( &md5 , &ssl->handshake->fin_md5 , sizeof(md5_context)  );
    memcpy( &sha1, &ssl->handshake->fin_sha1, sizeof(sha1_context) );

    /*
     * TLSv1:
     *   hash = PRF( master, finished_label,
     *               MD5( handshake ) + SHA1( handshake ) )[0..11]
     */

#if !defined(POLARSSL_MD5_ALT)
    SSL_DEBUG_BUF( 4, "finished  md5 state", (unsigned char *)
                    md5.state, sizeof(  md5.state ) );
#endif

#if !defined(POLARSSL_SHA1_ALT)
    SSL_DEBUG_BUF( 4, "finished sha1 state", (unsigned char *)
                   sha1.state, sizeof( sha1.state ) );
#endif

    sender = ( from == SSL_IS_CLIENT )
             ? "client finished"
             : "server finished";

    md5_finish(  &md5, padbuf );
    sha1_finish( &sha1, padbuf + 16 );

    ssl->handshake->tls_prf( session->master, 48, sender,
                             padbuf, 36, buf, len );

    SSL_DEBUG_BUF( 3, "calc finished result", buf, len );

    md5_free(  &md5  );
    sha1_free( &sha1 );

    polarssl_zeroize(  padbuf, sizeof(  padbuf ) );

    SSL_DEBUG_MSG( 2, ( "<= calc  finished" ) );
}