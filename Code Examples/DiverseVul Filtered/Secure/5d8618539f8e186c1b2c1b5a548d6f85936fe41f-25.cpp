int x509_dn_gets( char *buf, size_t size, const x509_name *dn )
{
    int ret;
    size_t i, n;
    unsigned char c;
    const x509_name *name;
    const char *short_name = NULL;
    char s[X509_MAX_DN_NAME_SIZE], *p;

    memset( s, 0, sizeof( s ) );

    name = dn;
    p = buf;
    n = size;

    while( name != NULL )
    {
        if( !name->oid.p )
        {
            name = name->next;
            continue;
        }

        if( name != dn )
        {
            ret = snprintf( p, n, ", " );
            SAFE_SNPRINTF();
        }

        ret = oid_get_attr_short_name( &name->oid, &short_name );

        if( ret == 0 )
            ret = snprintf( p, n, "%s=", short_name );
        else
            ret = snprintf( p, n, "\?\?=" );
        SAFE_SNPRINTF();

        for( i = 0; i < name->val.len; i++ )
        {
            if( i >= sizeof( s ) - 1 )
                break;

            c = name->val.p[i];
            if( c < 32 || c == 127 || ( c > 128 && c < 160 ) )
                 s[i] = '?';
            else s[i] = c;
        }
        s[i] = '\0';
        ret = snprintf( p, n, "%s", s );
        SAFE_SNPRINTF();
        name = name->next;
    }

    return( (int) ( size - n ) );
}