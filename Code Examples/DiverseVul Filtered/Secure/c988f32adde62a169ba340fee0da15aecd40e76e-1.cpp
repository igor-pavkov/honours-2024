void ssl_transform_free( ssl_transform *transform )
{
    if( transform == NULL )
        return;

#if defined(POLARSSL_ZLIB_SUPPORT)
    deflateEnd( &transform->ctx_deflate );
    inflateEnd( &transform->ctx_inflate );
#endif

    cipher_free( &transform->cipher_ctx_enc );
    cipher_free( &transform->cipher_ctx_dec );

    md_free( &transform->md_ctx_enc );
    md_free( &transform->md_ctx_dec );

    polarssl_zeroize( transform, sizeof( ssl_transform ) );
}