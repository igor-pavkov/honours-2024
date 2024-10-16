void ssl_transform_free( ssl_transform *transform )
{
#if defined(POLARSSL_ZLIB_SUPPORT)
    deflateEnd( &transform->ctx_deflate );
    inflateEnd( &transform->ctx_inflate );
#endif

    memset( transform, 0, sizeof( ssl_transform ) );
}