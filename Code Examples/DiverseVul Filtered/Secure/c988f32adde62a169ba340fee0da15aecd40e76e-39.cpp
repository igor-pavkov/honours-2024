int ssl_set_dh_param_ctx( ssl_context *ssl, dhm_context *dhm_ctx )
{
    int ret;

    if( ( ret = mpi_copy( &ssl->dhm_P, &dhm_ctx->P ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "mpi_copy", ret );
        return( ret );
    }

    if( ( ret = mpi_copy( &ssl->dhm_G, &dhm_ctx->G ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "mpi_copy", ret );
        return( ret );
    }

    return( 0 );
}