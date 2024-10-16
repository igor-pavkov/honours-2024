static void ssl_mac_sha2( unsigned char *secret,
                          unsigned char *buf, size_t len,
                          unsigned char *ctr, int type )
{
    unsigned char header[11];
    unsigned char padding[32];
    sha2_context sha2;

    memcpy( header, ctr, 8 );
    header[ 8] = (unsigned char)  type;
    header[ 9] = (unsigned char)( len >> 8 );
    header[10] = (unsigned char)( len      );

    memset( padding, 0x36, 32 );
    sha2_starts( &sha2, 0 );
    sha2_update( &sha2, secret,  32 );
    sha2_update( &sha2, padding, 32 );
    sha2_update( &sha2, header,  11 );
    sha2_update( &sha2, buf,  len );
    sha2_finish( &sha2, buf + len );

    memset( padding, 0x5C, 32 );
    sha2_starts( &sha2, 0 );
    sha2_update( &sha2, secret,  32 );
    sha2_update( &sha2, padding, 32 );
    sha2_update( &sha2, buf + len, 32 );
    sha2_finish( &sha2, buf + len );
}