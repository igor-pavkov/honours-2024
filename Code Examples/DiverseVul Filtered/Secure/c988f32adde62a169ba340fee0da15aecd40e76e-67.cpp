int ssl_curve_is_acceptable( const ssl_context *ssl, ecp_group_id grp_id )
{
    const ecp_group_id *gid;

    for( gid = ssl->curve_list; *gid != POLARSSL_ECP_DP_NONE; gid++ )
        if( *gid == grp_id )
            return( 1 );

    return( 0 );
}