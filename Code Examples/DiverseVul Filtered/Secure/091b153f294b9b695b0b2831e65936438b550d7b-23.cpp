int do_attack_interactive( void )
{
    int caplen, n, z;
    int mi_b, mi_s, mi_d;
    struct timeval tv;
    struct timeval tv2;
    float f, ticks[3];
    unsigned char bssid[6];
    unsigned char smac[6];
    unsigned char dmac[6];

read_packets:

    if( capture_ask_packet( &caplen, 0 ) != 0 )
        return( 1 );

    z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
    if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
        z+=2;

    /* rewrite the frame control & MAC addresses */

    switch( h80211[1] & 3 )
    {
        case  0: mi_b = 16; mi_s = 10; mi_d =  4; break;
        case  1: mi_b =  4; mi_s = 10; mi_d = 16; break;
        case  2: mi_b = 10; mi_s = 16; mi_d =  4; break;
        default: mi_b = 10; mi_d = 16; mi_s = 24; break;
    }

    if( memcmp( opt.r_bssid, NULL_MAC, 6 ) == 0 )
        memcpy( bssid, h80211 + mi_b, 6 );
    else
        memcpy( bssid, opt.r_bssid, 6 );

    if( memcmp( opt.r_smac , NULL_MAC, 6 ) == 0 )
        memcpy( smac, h80211 + mi_s, 6 );
    else
        memcpy( smac, opt.r_smac, 6 );

    if( memcmp( opt.r_dmac , NULL_MAC, 6 ) == 0 )
        memcpy( dmac, h80211 + mi_d, 6 );
    else
        memcpy( dmac, opt.r_dmac, 6 );

    if( opt.r_fctrl != -1 )
    {
        h80211[0] = opt.r_fctrl >>   8;
        h80211[1] = opt.r_fctrl & 0xFF;

        switch( h80211[1] & 3 )
        {
            case  0: mi_b = 16; mi_s = 10; mi_d =  4; break;
            case  1: mi_b =  4; mi_s = 10; mi_d = 16; break;
            case  2: mi_b = 10; mi_s = 16; mi_d =  4; break;
            default: mi_b = 10; mi_d = 16; mi_s = 24; break;
        }
    }

    memcpy( h80211 + mi_b, bssid, 6 );
    memcpy( h80211 + mi_s, smac , 6 );
    memcpy( h80211 + mi_d, dmac , 6 );

    /* loop resending the packet */

	/* Check if airodump-ng is running. If not, print that message */
    printf( "You should also start airodump-ng to capture replies.\n\n" );

    signal( SIGINT, sighandler );
    ctrl_c = 0;

    memset( ticks, 0, sizeof( ticks ) );

    nb_pkt_sent = 0;

    while( 1 )
    {
        if( ctrl_c )
            goto read_packets;

        /* wait for the next timer interrupt, or sleep */

        if( dev.fd_rtc >= 0 )
        {
            if( read( dev.fd_rtc, &n, sizeof( n ) ) < 0 )
            {
                perror( "read(/dev/rtc) failed" );
                return( 1 );
            }

            ticks[0]++;
            ticks[1]++;
            ticks[2]++;
        }
        else
        {
            /* we can't trust usleep, since it depends on the HZ */

            gettimeofday( &tv,  NULL );
            usleep( 1000000/RTC_RESOLUTION );
            gettimeofday( &tv2, NULL );

            f = 1000000 * (float) ( tv2.tv_sec  - tv.tv_sec  )
                        + (float) ( tv2.tv_usec - tv.tv_usec );

            ticks[0] += f / ( 1000000/RTC_RESOLUTION );
            ticks[1] += f / ( 1000000/RTC_RESOLUTION );
            ticks[2] += f / ( 1000000/RTC_RESOLUTION );
        }

        /* update the status line */

        if( ticks[1] > (RTC_RESOLUTION/10) )
        {
            ticks[1] = 0;
            printf( "\rSent %ld packets...(%d pps)\33[K\r", nb_pkt_sent, (int)((double)nb_pkt_sent/((double)ticks[0]/(double)RTC_RESOLUTION)));
            fflush( stdout );
        }

        if( ( ticks[2] * opt.r_nbpps ) / RTC_RESOLUTION < 1 )
            continue;

        /* threshold reached */

        ticks[2] = 0;

        if( nb_pkt_sent == 0 )
            ticks[0] = 0;

        if( send_packet( h80211, caplen ) < 0 )
            return( 1 );

        if( ((double)ticks[0]/(double)RTC_RESOLUTION)*(double)opt.r_nbpps > (double)nb_pkt_sent  )
        {
            if( send_packet( h80211, caplen ) < 0 )
                return( 1 );
        }
    }

    return( 0 );
}