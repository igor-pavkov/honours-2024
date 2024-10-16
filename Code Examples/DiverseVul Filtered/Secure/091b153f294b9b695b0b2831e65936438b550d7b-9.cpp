void read_sleep( int usec )
{
    struct timeval tv, tv2, tv3;
    int caplen;
    fd_set rfds;

    gettimeofday(&tv, NULL);
    gettimeofday(&tv2, NULL);

    tv3.tv_sec=0;
    tv3.tv_usec=10000;

    while( ((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) < (usec) )
    {
        FD_ZERO( &rfds );
        FD_SET( dev.fd_in, &rfds );

        if( select( dev.fd_in + 1, &rfds, NULL, NULL, &tv3 ) < 0 )
        {
            continue;
        }

        if( FD_ISSET( dev.fd_in, &rfds ) )
            caplen = read_packet( h80211, sizeof( h80211 ), NULL );

        gettimeofday(&tv2, NULL);
    }
}