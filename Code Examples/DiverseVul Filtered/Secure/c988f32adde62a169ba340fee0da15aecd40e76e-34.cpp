static int tls_prf_sha256( const unsigned char *secret, size_t slen,
                           const char *label,
                           const unsigned char *random, size_t rlen,
                           unsigned char *dstbuf, size_t dlen )
{
    size_t nb;
    size_t i, j, k;
    unsigned char tmp[128];
    unsigned char h_i[32];

    if( sizeof( tmp ) < 32 + strlen( label ) + rlen )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    nb = strlen( label );
    memcpy( tmp + 32, label, nb );
    memcpy( tmp + 32 + nb, random, rlen );
    nb += rlen;

    /*
     * Compute P_<hash>(secret, label + random)[0..dlen]
     */
    sha256_hmac( secret, slen, tmp + 32, nb, tmp, 0 );

    for( i = 0; i < dlen; i += 32 )
    {
        sha256_hmac( secret, slen, tmp, 32 + nb, h_i, 0 );
        sha256_hmac( secret, slen, tmp, 32,      tmp, 0 );

        k = ( i + 32 > dlen ) ? dlen % 32 : 32;

        for( j = 0; j < k; j++ )
            dstbuf[i + j]  = h_i[j];
    }

    polarssl_zeroize( tmp, sizeof( tmp ) );
    polarssl_zeroize( h_i, sizeof( h_i ) );

    return( 0 );
}