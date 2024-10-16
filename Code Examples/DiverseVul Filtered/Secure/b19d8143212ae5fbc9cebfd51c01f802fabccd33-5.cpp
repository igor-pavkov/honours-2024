int ssl3_send_server_done(SSL *s)
{

    if (s->state == SSL3_ST_SW_SRVR_DONE_A) {
        ssl_set_handshake_header(s, SSL3_MT_SERVER_DONE, 0);
        s->state = SSL3_ST_SW_SRVR_DONE_B;
    }

    /* SSL3_ST_SW_SRVR_DONE_B */
    return ssl_do_write(s);
}