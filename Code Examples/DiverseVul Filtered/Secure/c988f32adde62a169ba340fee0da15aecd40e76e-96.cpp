int ssl_psk_derive_premaster( ssl_context *ssl, key_exchange_type_t key_ex )
{
    unsigned char *p = ssl->handshake->premaster;
    unsigned char *end = p + sizeof( ssl->handshake->premaster );

    /*
     * PMS = struct {
     *     opaque other_secret<0..2^16-1>;
     *     opaque psk<0..2^16-1>;
     * };
     * with "other_secret" depending on the particular key exchange
     */
#if defined(POLARSSL_KEY_EXCHANGE_PSK_ENABLED)
    if( key_ex == POLARSSL_KEY_EXCHANGE_PSK )
    {
        if( end - p < 2 + (int) ssl->psk_len )
            return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

        *(p++) = (unsigned char)( ssl->psk_len >> 8 );
        *(p++) = (unsigned char)( ssl->psk_len      );
        p += ssl->psk_len;
    }
    else
#endif /* POLARSSL_KEY_EXCHANGE_PSK_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_RSA_PSK_ENABLED)
    if( key_ex == POLARSSL_KEY_EXCHANGE_RSA_PSK )
    {
        /*
         * other_secret already set by the ClientKeyExchange message,
         * and is 48 bytes long
         */
        *p++ = 0;
        *p++ = 48;
        p += 48;
    }
    else
#endif /* POLARSSL_KEY_EXCHANGE_RSA_PKS_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED)
    if( key_ex == POLARSSL_KEY_EXCHANGE_DHE_PSK )
    {
        int ret;
        size_t len = end - ( p + 2 );

        /* Write length only when we know the actual value */
        if( ( ret = dhm_calc_secret( &ssl->handshake->dhm_ctx,
                                      p + 2, &len,
                                      ssl->f_rng, ssl->p_rng ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "dhm_calc_secret", ret );
            return( ret );
        }
        *(p++) = (unsigned char)( len >> 8 );
        *(p++) = (unsigned char)( len );
        p += len;

        SSL_DEBUG_MPI( 3, "DHM: K ", &ssl->handshake->dhm_ctx.K  );
    }
    else
#endif /* POLARSSL_KEY_EXCHANGE_DHE_PSK_ENABLED */
#if defined(POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
    if( key_ex == POLARSSL_KEY_EXCHANGE_ECDHE_PSK )
    {
        int ret;
        size_t zlen;

        if( ( ret = ecdh_calc_secret( &ssl->handshake->ecdh_ctx, &zlen,
                                       p + 2, end - ( p + 2 ),
                                       ssl->f_rng, ssl->p_rng ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ecdh_calc_secret", ret );
            return( ret );
        }

        *(p++) = (unsigned char)( zlen >> 8 );
        *(p++) = (unsigned char)( zlen      );
        p += zlen;

        SSL_DEBUG_MPI( 3, "ECDH: z", &ssl->handshake->ecdh_ctx.z );
    }
    else
#endif /* POLARSSL_KEY_EXCHANGE_ECDHE_PSK_ENABLED */
    {
        SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
    }

    /* opaque psk<0..2^16-1>; */
    if( end - p < 2 + (int) ssl->psk_len )
            return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    *(p++) = (unsigned char)( ssl->psk_len >> 8 );
    *(p++) = (unsigned char)( ssl->psk_len      );
    memcpy( p, ssl->psk, ssl->psk_len );
    p += ssl->psk_len;

    ssl->handshake->pmslen = p - ssl->handshake->premaster;

    return( 0 );
}