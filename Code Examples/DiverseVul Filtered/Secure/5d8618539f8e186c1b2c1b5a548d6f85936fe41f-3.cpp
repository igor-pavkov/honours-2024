int x509_serial_gets( char *buf, size_t size, const x509_buf *serial )
{
    int ret;
    size_t i, n, nr;
    char *p;

    p = buf;
    n = size;

    nr = ( serial->len <= 32 )
        ? serial->len  : 28;

    for( i = 0; i < nr; i++ )
    {
        if( i == 0 && nr > 1 && serial->p[i] == 0x0 )
            continue;

        ret = snprintf( p, n, "%02X%s",
                serial->p[i], ( i < nr - 1 ) ? ":" : "" );
        SAFE_SNPRINTF();
    }

    if( nr != serial->len )
    {
        ret = snprintf( p, n, "...." );
        SAFE_SNPRINTF();
    }

    return( (int) ( size - n ) );
}