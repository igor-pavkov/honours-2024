int x509_time_future( const x509_time *from )
{
    x509_time now;

    x509_get_current_time( &now );

    return( x509_check_time( from, &now ) );
}