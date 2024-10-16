int x509_get_time( unsigned char **p, const unsigned char *end,
                   x509_time *time )
{
    int ret;
    size_t len;
    char date[64];
    unsigned char tag;

    if( ( end - *p ) < 1 )
        return( POLARSSL_ERR_X509_INVALID_DATE +
                POLARSSL_ERR_ASN1_OUT_OF_DATA );

    tag = **p;

    if( tag == ASN1_UTC_TIME )
    {
        (*p)++;
        ret = asn1_get_len( p, end, &len );

        if( ret != 0 )
            return( POLARSSL_ERR_X509_INVALID_DATE + ret );

        memset( date,  0, sizeof( date ) );
        memcpy( date, *p, ( len < sizeof( date ) - 1 ) ?
                len : sizeof( date ) - 1 );

        if( sscanf( date, "%2d%2d%2d%2d%2d%2dZ",
                    &time->year, &time->mon, &time->day,
                    &time->hour, &time->min, &time->sec ) < 5 )
            return( POLARSSL_ERR_X509_INVALID_DATE );

        time->year +=  100 * ( time->year < 50 );
        time->year += 1900;

        *p += len;

        return( 0 );
    }
    else if( tag == ASN1_GENERALIZED_TIME )
    {
        (*p)++;
        ret = asn1_get_len( p, end, &len );

        if( ret != 0 )
            return( POLARSSL_ERR_X509_INVALID_DATE + ret );

        memset( date,  0, sizeof( date ) );
        memcpy( date, *p, ( len < sizeof( date ) - 1 ) ?
                len : sizeof( date ) - 1 );

        if( sscanf( date, "%4d%2d%2d%2d%2d%2dZ",
                    &time->year, &time->mon, &time->day,
                    &time->hour, &time->min, &time->sec ) < 5 )
            return( POLARSSL_ERR_X509_INVALID_DATE );

        *p += len;

        return( 0 );
    }
    else
        return( POLARSSL_ERR_X509_INVALID_DATE +
                POLARSSL_ERR_ASN1_UNEXPECTED_TAG );
}