md_type_t ssl_md_alg_from_hash( unsigned char hash )
{
    switch( hash )
    {
#if defined(POLARSSL_MD5_C)
        case SSL_HASH_MD5:
            return( POLARSSL_MD_MD5 );
#endif
#if defined(POLARSSL_SHA1_C)
        case SSL_HASH_SHA1:
            return( POLARSSL_MD_SHA1 );
#endif
#if defined(POLARSSL_SHA256_C)
        case SSL_HASH_SHA224:
            return( POLARSSL_MD_SHA224 );
        case SSL_HASH_SHA256:
            return( POLARSSL_MD_SHA256 );
#endif
#if defined(POLARSSL_SHA512_C)
        case SSL_HASH_SHA384:
            return( POLARSSL_MD_SHA384 );
        case SSL_HASH_SHA512:
            return( POLARSSL_MD_SHA512 );
#endif
        default:
            return( POLARSSL_MD_NONE );
    }
}