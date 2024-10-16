int capture_ask_packet( int *caplen, int just_grab )
{
    time_t tr;
    struct timeval tv;
    struct tm *lt;

    fd_set rfds;
    long nb_pkt_read;
    int i, j, n, mi_b=0, mi_s=0, mi_d=0, mi_t=0, mi_r=0, is_wds=0, key_index_offset;
    int ret, z;

    FILE *f_cap_out;
    struct pcap_file_header pfh_out;
    struct pcap_pkthdr pkh;

    if( opt.f_minlen  < 0 ) opt.f_minlen  =   40;
    if( opt.f_maxlen  < 0 ) opt.f_maxlen  = 1500;
    if( opt.f_type    < 0 ) opt.f_type    =    2;
    if( opt.f_subtype < 0 ) opt.f_subtype =    0;
    if( opt.f_iswep   < 0 ) opt.f_iswep   =    1;

    tr = time( NULL );

    nb_pkt_read = 0;

    signal( SIGINT, SIG_DFL );

    while( 1 )
    {
        if( time( NULL ) - tr > 0 )
        {
            tr = time( NULL );
            printf( "\rRead %ld packets...\r", nb_pkt_read );
            fflush( stdout );
        }

        if( opt.s_file == NULL )
        {
            FD_ZERO( &rfds );
            FD_SET( dev.fd_in, &rfds );

            tv.tv_sec  = 1;
            tv.tv_usec = 0;

            if( select( dev.fd_in + 1, &rfds, NULL, NULL, &tv ) < 0 )
            {
                if( errno == EINTR ) continue;
                perror( "select failed" );
                return( 1 );
            }

            if( ! FD_ISSET( dev.fd_in, &rfds ) )
                continue;

            gettimeofday( &tv, NULL );

            *caplen = read_packet( h80211, sizeof( h80211 ), NULL );

            if( *caplen  < 0 ) return( 1 );
            if( *caplen == 0 ) continue;
        }
        else
        {
            /* there are no hidden backdoors in this source code */

            n = sizeof( pkh );

            if( fread( &pkh, n, 1, dev.f_cap_in ) != 1 )
            {
                printf( "\r\33[KEnd of file.\n" );
                return( 1 );
            }

            if( dev.pfh_in.magic == TCPDUMP_CIGAM ) {
                SWAP32( pkh.caplen );
                SWAP32( pkh.len );
            }

            tv.tv_sec  = pkh.tv_sec;
            tv.tv_usec = pkh.tv_usec;

            n = *caplen = pkh.caplen;

            if( n <= 0 || n > (int) sizeof( h80211 ) || n > (int) sizeof( tmpbuf ) )
            {
                printf( "\r\33[KInvalid packet length %d.\n", n );
                return( 1 );
            }

            if( fread( h80211, n, 1, dev.f_cap_in ) != 1 )
            {
                printf( "\r\33[KEnd of file.\n" );
                return( 1 );
            }

            if( dev.pfh_in.linktype == LINKTYPE_PRISM_HEADER )
            {
                /* remove the prism header */

                if( h80211[7] == 0x40 )
                    n = 64;
                else
                    n = *(int *)( h80211 + 4 );

                if( n < 8 || n >= (int) *caplen )
                    continue;

                memcpy( tmpbuf, h80211, *caplen );
                *caplen -= n;
                memcpy( h80211, tmpbuf + n, *caplen );
            }

            if( dev.pfh_in.linktype == LINKTYPE_RADIOTAP_HDR )
            {
                /* remove the radiotap header */

                n = *(unsigned short *)( h80211 + 2 );

                if( n <= 0 || n >= (int) *caplen )
                    continue;

                memcpy( tmpbuf, h80211, *caplen );
                *caplen -= n;
                memcpy( h80211, tmpbuf + n, *caplen );
            }

            if( dev.pfh_in.linktype == LINKTYPE_PPI_HDR )
            {
                /* remove the PPI header */

                n = le16_to_cpu(*(unsigned short *)( h80211 + 2));

                if( n <= 0 || n>= (int) *caplen )
                    continue;

                /* for a while Kismet logged broken PPI headers */
                if ( n == 24 && le16_to_cpu(*(unsigned short *)(h80211 + 8)) == 2 )
                    n = 32;

                if( n <= 0 || n>= (int) *caplen )
                    continue;

                memcpy( tmpbuf, h80211, *caplen );
                *caplen -= n;
                memcpy( h80211, tmpbuf + n, *caplen );
            }
        }

        nb_pkt_read++;

        if( filter_packet( h80211, *caplen ) != 0 )
            continue;

        if(opt.fast)
            break;

        z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
        if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
            z+=2;

        switch( h80211[1] & 3 )
        {
            case  0: mi_b = 16; mi_s = 10; mi_d =  4; is_wds = 0; break;
            case  1: mi_b =  4; mi_s = 10; mi_d = 16; is_wds = 0; break;
            case  2: mi_b = 10; mi_s = 16; mi_d =  4; is_wds = 0; break;
            case  3: mi_t = 10; mi_r =  4; mi_d = 16; mi_s = 24; is_wds = 1; break;  // WDS packet
        }

        printf( "\n\n        Size: %d, FromDS: %d, ToDS: %d",
                *caplen, ( h80211[1] & 2 ) >> 1, ( h80211[1] & 1 ) );

        if( ( h80211[0] & 0x0C ) == 8 && ( h80211[1] & 0x40 ) != 0 )
        {
//             if (is_wds) key_index_offset = 33; // WDS packets have an additional MAC, so the key index is at byte 33
//             else key_index_offset = 27;
            key_index_offset = z+3;

            if( ( h80211[key_index_offset] & 0x20 ) == 0 )
                printf( " (WEP)" );
            else
                printf( " (WPA)" );
        }

        printf( "\n\n" );

        if (is_wds) {
            printf( "        Transmitter  =  %02X:%02X:%02X:%02X:%02X:%02X\n",
                    h80211[mi_t    ], h80211[mi_t + 1],
                    h80211[mi_t + 2], h80211[mi_t + 3],
                    h80211[mi_t + 4], h80211[mi_t + 5] );

            printf( "           Receiver  =  %02X:%02X:%02X:%02X:%02X:%02X\n",
                    h80211[mi_r    ], h80211[mi_r + 1],
                    h80211[mi_r + 2], h80211[mi_r + 3],
                    h80211[mi_r + 4], h80211[mi_r + 5] );
        } else {
            printf( "              BSSID  =  %02X:%02X:%02X:%02X:%02X:%02X\n",
                    h80211[mi_b    ], h80211[mi_b + 1],
                    h80211[mi_b + 2], h80211[mi_b + 3],
                    h80211[mi_b + 4], h80211[mi_b + 5] );
        }

        printf( "          Dest. MAC  =  %02X:%02X:%02X:%02X:%02X:%02X\n",
                h80211[mi_d    ], h80211[mi_d + 1],
                h80211[mi_d + 2], h80211[mi_d + 3],
                h80211[mi_d + 4], h80211[mi_d + 5] );

        printf( "         Source MAC  =  %02X:%02X:%02X:%02X:%02X:%02X\n",
                h80211[mi_s    ], h80211[mi_s + 1],
                h80211[mi_s + 2], h80211[mi_s + 3],
                h80211[mi_s + 4], h80211[mi_s + 5] );

        /* print a hex dump of the packet */

        for( i = 0; i < *caplen; i++ )
        {
            if( ( i & 15 ) == 0 )
            {
                if( i == 224 )
                {
                    printf( "\n        --- CUT ---" );
                    break;
                }

                printf( "\n        0x%04x:  ", i );
            }

            printf( "%02x", h80211[i] );

            if( ( i & 1 ) != 0 )
                printf( " " );

            if( i == *caplen - 1 && ( ( i + 1 ) & 15 ) != 0 )
            {
                for( j = ( ( i + 1 ) & 15 ); j < 16; j++ )
                {
                    printf( "  " );
                    if( ( j & 1 ) != 0 )
                        printf( " " );
                }

                printf( " " );

                for( j = 16 - ( ( i + 1 ) & 15 ); j < 16; j++ )
                    printf( "%c", ( h80211[i - 15 + j] <  32 ||
                                    h80211[i - 15 + j] > 126 )
                                  ? '.' : h80211[i - 15 + j] );
            }

            if( i > 0 && ( ( i + 1 ) & 15 ) == 0 )
            {
                printf( " " );

                for( j = 0; j < 16; j++ )
                    printf( "%c", ( h80211[i - 15 + j] <  32 ||
                                    h80211[i - 15 + j] > 127 )
                                  ? '.' : h80211[i - 15 + j] );
            }
        }

        printf( "\n\nUse this packet ? " );
        fflush( stdout );
        ret=0;
        while(!ret) ret = scanf( "%s", tmpbuf );
        printf( "\n" );

        if( tmpbuf[0] == 'y' || tmpbuf[0] == 'Y' )
            break;
    }

    if(!just_grab)
    {
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
                "replay_src-%02d%02d-%02d%02d%02d.cap",
                lt->tm_mon + 1, lt->tm_mday,
                lt->tm_hour, lt->tm_min, lt->tm_sec );

        printf( "Saving chosen packet in %s\n", strbuf );

        if( ( f_cap_out = fopen( strbuf, "wb+" ) ) == NULL )
        {
            perror( "fopen failed" );
            return( 1 );
        }

        n = sizeof( struct pcap_file_header );

        if( fwrite( &pfh_out, n, 1, f_cap_out ) != 1 )
        {
        	fclose(f_cap_out);
            perror( "fwrite failed\n" );
            return( 1 );
        }

        pkh.tv_sec  = tv.tv_sec;
        pkh.tv_usec = tv.tv_usec;
        pkh.caplen  = *caplen;
        pkh.len     = *caplen;

        n = sizeof( pkh );

        if( fwrite( &pkh, n, 1, f_cap_out ) != 1 )
        {
        	fclose(f_cap_out);
            perror( "fwrite failed" );
            return( 1 );
        }

        n = pkh.caplen;

        if( fwrite( h80211, n, 1, f_cap_out ) != 1 )
        {
        	fclose(f_cap_out);
            perror( "fwrite failed" );
            return( 1 );
        }

        fclose( f_cap_out );
    }

    return( 0 );
}