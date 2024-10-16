static void ssl_mac_md5( unsigned char *secret,
                         unsigned char *buf, size_t len,
                         unsigned char *ctr, int type )
{
    unsigned char header[11];
    unsigned char padding[48];
    md5_context md5;

    memcpy( header, ctr, 8 );
    header[ 8] = (unsigned char)  type;
    header[ 9] = (unsigned char)( len >> 8 );
    header[10] = (unsigned char)( len      );

    memset( padding, 0x36, 48 );
    md5_starts( &md5 );
    md5_update( &md5, secret,  16 );
    md5_update( &md5, padding, 48 );
    md5_update( &md5, header,  11 );
    md5_update( &md5, buf,  len );
    md5_finish( &md5, buf + len );

    memset( padding, 0x5C, 48 );
    md5_starts( &md5 );
    md5_update( &md5, secret,  16 );
    md5_update( &md5, padding, 48 );
    md5_update( &md5, buf + len, 16 );
    md5_finish( &md5, buf + len );
}