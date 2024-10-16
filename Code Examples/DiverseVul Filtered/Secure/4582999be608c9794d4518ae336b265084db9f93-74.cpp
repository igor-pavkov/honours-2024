int ssl_set_dh_param( ssl_context *ssl, const char *dhm_P, const char *dhm_G )
{
    int ret;

    if( ( ret = mpi_read_string( &ssl->dhm_P, 16, dhm_P ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "mpi_read_string", ret );
        return( ret );
    }

    if( ( ret = mpi_read_string( &ssl->dhm_G, 16, dhm_G ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "mpi_read_string", ret );
        return( ret );
    }

    return( 0 );
}