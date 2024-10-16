void closedata(void)
{
    volatile int tmp_xferfd = xferfd;   /* do not simplify this... */

#ifdef WITH_TLS
    tls_close_session(&tls_data_cnx);
    tls_data_cnx = NULL;
#endif
    xferfd = -1;           /* ...it avoids a race */
    (void) close(tmp_xferfd);
}