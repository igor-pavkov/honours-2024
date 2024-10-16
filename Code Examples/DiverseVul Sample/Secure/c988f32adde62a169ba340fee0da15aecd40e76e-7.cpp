static void ssl_transform_init( ssl_transform *transform )
{
    memset( transform, 0, sizeof(ssl_transform) );

    cipher_init( &transform->cipher_ctx_enc );
    cipher_init( &transform->cipher_ctx_dec );

    md_init( &transform->md_ctx_enc );
    md_init( &transform->md_ctx_dec );
}