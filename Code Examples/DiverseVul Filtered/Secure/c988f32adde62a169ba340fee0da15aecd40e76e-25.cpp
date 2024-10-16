static int tls_prf_sha384( const unsigned char *secret, size_t slen,
                           const char *label,
                           const unsigned char *random, size_t rlen,
                           unsigned char *dstbuf, size_t dlen )
{
    size_t nb;
    size_t i, j, k;
    unsigned char tmp[128];
    unsigned char h_i[48];

    if( sizeof( tmp ) < 48 + strlen( label ) + rlen )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    nb = strlen( label );
    memcpy( tmp + 48, label, nb );
    memcpy( tmp + 48 + nb, random, rlen );
    nb += rlen;

    /*
     * Compute P_<hash>(secret, label + random)[0..dlen]
     */
    sha512_hmac( secret, slen, tmp + 48, nb, tmp, 1 );

    for( i = 0; i < dlen; i += 48 )
    {
        sha512_hmac( secret, slen, tmp, 48 + nb, h_i, 1 );
        sha512_hmac( secret, slen, tmp, 48,      tmp, 1 );

        k = ( i + 48 > dlen ) ? dlen % 48 : 48;

        for( j = 0; j < k; j++ )
            dstbuf[i + j]  = h_i[j];
    }

    polarssl_zeroize( tmp, sizeof( tmp ) );
    polarssl_zeroize( h_i, sizeof( h_i ) );

    return( 0 );
}