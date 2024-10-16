static void ssl_mac( md_context_t *md_ctx, unsigned char *secret,
                     unsigned char *buf, size_t len,
                     unsigned char *ctr, int type )
{
    unsigned char header[11];
    unsigned char padding[48];
    int padlen;
    int md_size = md_get_size( md_ctx->md_info );
    int md_type = md_get_type( md_ctx->md_info );

    /* Only MD5 and SHA-1 supported */
    if( md_type == POLARSSL_MD_MD5 )
        padlen = 48;
    else
        padlen = 40;

    memcpy( header, ctr, 8 );
    header[ 8] = (unsigned char)  type;
    header[ 9] = (unsigned char)( len >> 8 );
    header[10] = (unsigned char)( len      );

    memset( padding, 0x36, padlen );
    md_starts( md_ctx );
    md_update( md_ctx, secret,  md_size );
    md_update( md_ctx, padding, padlen  );
    md_update( md_ctx, header,  11      );
    md_update( md_ctx, buf,     len     );
    md_finish( md_ctx, buf +    len     );

    memset( padding, 0x5C, padlen );
    md_starts( md_ctx );
    md_update( md_ctx, secret,    md_size );
    md_update( md_ctx, padding,   padlen  );
    md_update( md_ctx, buf + len, md_size );
    md_finish( md_ctx, buf + len          );
}