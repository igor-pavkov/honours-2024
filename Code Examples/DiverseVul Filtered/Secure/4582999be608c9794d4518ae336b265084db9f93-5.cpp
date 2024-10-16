static int ssl3_prf( unsigned char *secret, size_t slen, char *label,
                     unsigned char *random, size_t rlen,
                     unsigned char *dstbuf, size_t dlen )
{
    size_t i;
    md5_context md5;
    sha1_context sha1;
    unsigned char padding[16];
    unsigned char sha1sum[20];
    ((void)label);

    /*
     *  SSLv3:
     *    block =
     *      MD5( secret + SHA1( 'A'    + secret + random ) ) +
     *      MD5( secret + SHA1( 'BB'   + secret + random ) ) +
     *      MD5( secret + SHA1( 'CCC'  + secret + random ) ) +
     *      ...
     */
    for( i = 0; i < dlen / 16; i++ )
    {
        memset( padding, 'A' + i, 1 + i );

        sha1_starts( &sha1 );
        sha1_update( &sha1, padding, 1 + i );
        sha1_update( &sha1, secret, slen );
        sha1_update( &sha1, random, rlen );
        sha1_finish( &sha1, sha1sum );

        md5_starts( &md5 );
        md5_update( &md5, secret, slen );
        md5_update( &md5, sha1sum, 20 );
        md5_finish( &md5, dstbuf + i * 16 );
    }

    memset( &md5,  0, sizeof( md5  ) );
    memset( &sha1, 0, sizeof( sha1 ) );

    memset( padding, 0, sizeof( padding ) );
    memset( sha1sum, 0, sizeof( sha1sum ) );

    return( 0 );
}