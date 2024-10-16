const char *x509_oid_get_description( x509_buf *oid )
{
    const char *desc = NULL;
    int ret;

    ret = oid_get_extended_key_usage( oid, &desc );

    if( ret != 0 )
        return( NULL );

    return( desc );
}