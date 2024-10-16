int ssl_write_certificate( ssl_context *ssl )
{
    int ret;
    size_t i, n;
    const x509_cert *crt;

    SSL_DEBUG_MSG( 2, ( "=> write certificate" ) );

    if( ssl->endpoint == SSL_IS_CLIENT )
    {
        if( ssl->client_auth == 0 )
        {
            SSL_DEBUG_MSG( 2, ( "<= skip write certificate" ) );
            ssl->state++;
            return( 0 );
        }

        /*
         * If using SSLv3 and got no cert, send an Alert message
         * (otherwise an empty Certificate message will be sent).
         */
        if( ssl->own_cert  == NULL &&
            ssl->minor_ver == SSL_MINOR_VERSION_0 )
        {
            ssl->out_msglen  = 2;
            ssl->out_msgtype = SSL_MSG_ALERT;
            ssl->out_msg[0]  = SSL_ALERT_LEVEL_WARNING;
            ssl->out_msg[1]  = SSL_ALERT_MSG_NO_CERT;

            SSL_DEBUG_MSG( 2, ( "got no certificate to send" ) );
            goto write_msg;
        }
    }
    else /* SSL_IS_SERVER */
    {
        if( ssl->own_cert == NULL )
        {
            SSL_DEBUG_MSG( 1, ( "got no certificate to send" ) );
            return( POLARSSL_ERR_SSL_CERTIFICATE_REQUIRED );
        }
    }

    SSL_DEBUG_CRT( 3, "own certificate", ssl->own_cert );

    /*
     *     0  .  0    handshake type
     *     1  .  3    handshake length
     *     4  .  6    length of all certs
     *     7  .  9    length of cert. 1
     *    10  . n-1   peer certificate
     *     n  . n+2   length of cert. 2
     *    n+3 . ...   upper level cert, etc.
     */
    i = 7;
    crt = ssl->own_cert;

    while( crt != NULL )
    {
        n = crt->raw.len;
        if( i + 3 + n > SSL_MAX_CONTENT_LEN )
        {
            SSL_DEBUG_MSG( 1, ( "certificate too large, %d > %d",
                           i + 3 + n, SSL_MAX_CONTENT_LEN ) );
            return( POLARSSL_ERR_SSL_CERTIFICATE_TOO_LARGE );
        }

        ssl->out_msg[i    ] = (unsigned char)( n >> 16 );
        ssl->out_msg[i + 1] = (unsigned char)( n >>  8 );
        ssl->out_msg[i + 2] = (unsigned char)( n       );

        i += 3; memcpy( ssl->out_msg + i, crt->raw.p, n );
        i += n; crt = crt->next;
    }

    ssl->out_msg[4]  = (unsigned char)( ( i - 7 ) >> 16 );
    ssl->out_msg[5]  = (unsigned char)( ( i - 7 ) >>  8 );
    ssl->out_msg[6]  = (unsigned char)( ( i - 7 )       );

    ssl->out_msglen  = i;
    ssl->out_msgtype = SSL_MSG_HANDSHAKE;
    ssl->out_msg[0]  = SSL_HS_CERTIFICATE;

write_msg:

    ssl->state++;

    if( ( ret = ssl_write_record( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_write_record", ret );
        return( ret );
    }

    SSL_DEBUG_MSG( 2, ( "<= write certificate" ) );

    return( 0 );
}