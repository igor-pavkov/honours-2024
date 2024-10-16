static void ssl_calc_finished_ssl(
                ssl_context *ssl, unsigned char *buf, int from )
{
    const char *sender;
    md5_context  md5;
    sha1_context sha1;

    unsigned char padbuf[48];
    unsigned char md5sum[16];
    unsigned char sha1sum[20];

    ssl_session *session = ssl->session_negotiate;
    if( !session )
        session = ssl->session;

    SSL_DEBUG_MSG( 2, ( "=> calc  finished ssl" ) );

    memcpy( &md5 , &ssl->handshake->fin_md5 , sizeof(md5_context)  );
    memcpy( &sha1, &ssl->handshake->fin_sha1, sizeof(sha1_context) );

    /*
     * SSLv3:
     *   hash =
     *      MD5( master + pad2 +
     *          MD5( handshake + sender + master + pad1 ) )
     *   + SHA1( master + pad2 +
     *         SHA1( handshake + sender + master + pad1 ) )
     */

#if !defined(POLARSSL_MD5_ALT)
    SSL_DEBUG_BUF( 4, "finished  md5 state", (unsigned char *)
                    md5.state, sizeof(  md5.state ) );
#endif

#if !defined(POLARSSL_SHA1_ALT)
    SSL_DEBUG_BUF( 4, "finished sha1 state", (unsigned char *)
                   sha1.state, sizeof( sha1.state ) );
#endif

    sender = ( from == SSL_IS_CLIENT ) ? "CLNT"
                                       : "SRVR";

    memset( padbuf, 0x36, 48 );

    md5_update( &md5, (const unsigned char *) sender, 4 );
    md5_update( &md5, session->master, 48 );
    md5_update( &md5, padbuf, 48 );
    md5_finish( &md5, md5sum );

    sha1_update( &sha1, (const unsigned char *) sender, 4 );
    sha1_update( &sha1, session->master, 48 );
    sha1_update( &sha1, padbuf, 40 );
    sha1_finish( &sha1, sha1sum );

    memset( padbuf, 0x5C, 48 );

    md5_starts( &md5 );
    md5_update( &md5, session->master, 48 );
    md5_update( &md5, padbuf, 48 );
    md5_update( &md5, md5sum, 16 );
    md5_finish( &md5, buf );

    sha1_starts( &sha1 );
    sha1_update( &sha1, session->master, 48 );
    sha1_update( &sha1, padbuf , 40 );
    sha1_update( &sha1, sha1sum, 20 );
    sha1_finish( &sha1, buf + 16 );

    SSL_DEBUG_BUF( 3, "calc finished result", buf, 36 );

    md5_free(  &md5  );
    sha1_free( &sha1 );

    polarssl_zeroize(  padbuf, sizeof(  padbuf ) );
    polarssl_zeroize(  md5sum, sizeof(  md5sum ) );
    polarssl_zeroize( sha1sum, sizeof( sha1sum ) );

    SSL_DEBUG_MSG( 2, ( "<= calc  finished" ) );
}