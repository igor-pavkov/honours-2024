static int ssl3_prf( const unsigned char *secret, size_t slen,
                     const char *label,
                     const unsigned char *random, size_t rlen,
                     unsigned char *dstbuf, size_t dlen )
{
    size_t i;
    md5_context md5;
    sha1_context sha1;
    unsigned char padding[16];
    unsigned char sha1sum[20];
    ((void)label);

    md5_init(  &md5  );
    sha1_init( &sha1 );

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
        memset( padding, (unsigned char) ('A' + i), 1 + i );

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

    md5_free(  &md5  );
    sha1_free( &sha1 );

    polarssl_zeroize( padding, sizeof( padding ) );
    polarssl_zeroize( sha1sum, sizeof( sha1sum ) );

    return( 0 );
}