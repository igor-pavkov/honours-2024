int x509_time_expired( const x509_time *to )
{
    x509_time now;

    x509_get_current_time( &now );

    return( x509_check_time( &now, to ) );
}