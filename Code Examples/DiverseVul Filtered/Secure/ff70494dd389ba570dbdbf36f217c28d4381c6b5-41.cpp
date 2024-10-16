int dump_add_packet( unsigned char *h80211, int caplen, struct rx_info *ri, int cardnum )
{
    int i, n, seq, msd, dlen, offset, clen, o;
    unsigned z;
    int type, length, numuni=0, numauth=0;
    struct pcap_pkthdr pkh;
    struct timeval tv;
    struct ivs2_pkthdr ivs2;
    unsigned char *p, *org_p, c;
    unsigned char bssid[6];
    unsigned char stmac[6];
    unsigned char namac[6];
    unsigned char clear[2048];
    int weight[16];
    int num_xor=0;

    struct AP_info *ap_cur = NULL;
    struct ST_info *st_cur = NULL;
    struct NA_info *na_cur = NULL;
    struct AP_info *ap_prv = NULL;
    struct ST_info *st_prv = NULL;
    struct NA_info *na_prv = NULL;

    /* skip all non probe response frames in active scanning simulation mode */
    if( G.active_scan_sim > 0 && h80211[0] != 0x50 )
        return(0);

    /* skip packets smaller than a 802.11 header */

    if( caplen < 24 )
        goto write_packet;

    /* skip (uninteresting) control frames */

    if( ( h80211[0] & 0x0C ) == 0x04 )
        goto write_packet;

    /* if it's a LLC null packet, just forget it (may change in the future) */

    if ( caplen > 28)
        if ( memcmp(h80211 + 24, llcnull, 4) == 0)
            return ( 0 );

    /* grab the sequence number */
    seq = ((h80211[22]>>4)+(h80211[23]<<4));

    /* locate the access point's MAC address */

    switch( h80211[1] & 3 )
    {
        case  0: memcpy( bssid, h80211 + 16, 6 ); break;  //Adhoc
        case  1: memcpy( bssid, h80211 +  4, 6 ); break;  //ToDS
        case  2: memcpy( bssid, h80211 + 10, 6 ); break;  //FromDS
        case  3: memcpy( bssid, h80211 + 10, 6 ); break;  //WDS -> Transmitter taken as BSSID
    }

    if( memcmp(G.f_bssid, NULL_MAC, 6) != 0 )
    {
        if( memcmp(G.f_netmask, NULL_MAC, 6) != 0 )
        {
            if(is_filtered_netmask(bssid)) return(1);
        }
        else
        {
            if( memcmp(G.f_bssid, bssid, 6) != 0 ) return(1);
        }
    }

    /* update our chained list of access points */

    ap_cur = G.ap_1st;
    ap_prv = NULL;

    while( ap_cur != NULL )
    {
        if( ! memcmp( ap_cur->bssid, bssid, 6 ) )
            break;

        ap_prv = ap_cur;
        ap_cur = ap_cur->next;
    }

    /* if it's a new access point, add it */

    if( ap_cur == NULL )
    {
        if( ! ( ap_cur = (struct AP_info *) malloc(
                         sizeof( struct AP_info ) ) ) )
        {
            perror( "malloc failed" );
            return( 1 );
        }

        /* if mac is listed as unknown, remove it */
        remove_namac(bssid);

        memset( ap_cur, 0, sizeof( struct AP_info ) );

        if( G.ap_1st == NULL )
            G.ap_1st = ap_cur;
        else
            ap_prv->next  = ap_cur;

        memcpy( ap_cur->bssid, bssid, 6 );
		if (ap_cur->manuf == NULL) {
			ap_cur->manuf = get_manufacturer(ap_cur->bssid[0], ap_cur->bssid[1], ap_cur->bssid[2]);
		}

        ap_cur->prev = ap_prv;

        ap_cur->tinit = time( NULL );
        ap_cur->tlast = time( NULL );

        ap_cur->avg_power   = -1;
        ap_cur->best_power  = -1;
        ap_cur->power_index = -1;

        for( i = 0; i < NB_PWR; i++ )
            ap_cur->power_lvl[i] = -1;

        ap_cur->channel    = -1;
        ap_cur->max_speed  = -1;
        ap_cur->security   = 0;

        ap_cur->uiv_root = uniqueiv_init();

        ap_cur->nb_dataps = 0;
        ap_cur->nb_data_old = 0;
        gettimeofday(&(ap_cur->tv), NULL);

        ap_cur->dict_started = 0;

        ap_cur->key = NULL;

        G.ap_end = ap_cur;

        ap_cur->nb_bcn     = 0;

        ap_cur->rx_quality = 0;
        ap_cur->fcapt      = 0;
        ap_cur->fmiss      = 0;
        ap_cur->last_seq   = 0;
        gettimeofday( &(ap_cur->ftimef), NULL);
        gettimeofday( &(ap_cur->ftimel), NULL);
        gettimeofday( &(ap_cur->ftimer), NULL);

        ap_cur->ssid_length = 0;
        ap_cur->essid_stored = 0;
        ap_cur->timestamp = 0;

        ap_cur->decloak_detect=G.decloak;
        ap_cur->is_decloak = 0;
        ap_cur->packets = NULL;

	ap_cur->marked = 0;
	ap_cur->marked_color = 1;

        ap_cur->data_root = NULL;
        ap_cur->EAP_detected = 0;
        memcpy(ap_cur->gps_loc_min, G.gps_loc, sizeof(float)*5);
        memcpy(ap_cur->gps_loc_max, G.gps_loc, sizeof(float)*5);
        memcpy(ap_cur->gps_loc_best, G.gps_loc, sizeof(float)*5);
    }

    /* update the last time seen */

    ap_cur->tlast = time( NULL );

    /* only update power if packets comes from
     * the AP: either type == mgmt and SA != BSSID,
     * or FromDS == 1 and ToDS == 0 */

    if( ( ( h80211[1] & 3 ) == 0 &&
            memcmp( h80211 + 10, bssid, 6 ) == 0 ) ||
        ( ( h80211[1] & 3 ) == 2 ) )
    {
        ap_cur->power_index = ( ap_cur->power_index + 1 ) % NB_PWR;
        ap_cur->power_lvl[ap_cur->power_index] = ri->ri_power;

        ap_cur->avg_power = 0;

        for( i = 0, n = 0; i < NB_PWR; i++ )
        {
            if( ap_cur->power_lvl[i] != -1 )
            {
                ap_cur->avg_power += ap_cur->power_lvl[i];
                n++;
            }
        }

        if( n > 0 )
        {
            ap_cur->avg_power /= n;
            if( ap_cur->avg_power > ap_cur->best_power )
            {
                ap_cur->best_power = ap_cur->avg_power;
                memcpy(ap_cur->gps_loc_best, G.gps_loc, sizeof(float)*5);
            }
        }
        else
            ap_cur->avg_power = -1;

        /* every packet in here comes from the AP */

        if(G.gps_loc[0] > ap_cur->gps_loc_max[0])
            ap_cur->gps_loc_max[0] = G.gps_loc[0];
        if(G.gps_loc[1] > ap_cur->gps_loc_max[1])
            ap_cur->gps_loc_max[1] = G.gps_loc[1];
        if(G.gps_loc[2] > ap_cur->gps_loc_max[2])
            ap_cur->gps_loc_max[2] = G.gps_loc[2];

        if(G.gps_loc[0] < ap_cur->gps_loc_min[0])
            ap_cur->gps_loc_min[0] = G.gps_loc[0];
        if(G.gps_loc[1] < ap_cur->gps_loc_min[1])
            ap_cur->gps_loc_min[1] = G.gps_loc[1];
        if(G.gps_loc[2] < ap_cur->gps_loc_min[2])
            ap_cur->gps_loc_min[2] = G.gps_loc[2];
//        printf("seqnum: %i\n", seq);

        if(ap_cur->fcapt == 0 && ap_cur->fmiss == 0) gettimeofday( &(ap_cur->ftimef), NULL);
        if(ap_cur->last_seq != 0) ap_cur->fmiss += (seq - ap_cur->last_seq - 1);
        ap_cur->last_seq = seq;
        ap_cur->fcapt++;
        gettimeofday( &(ap_cur->ftimel), NULL);

//         if(ap_cur->fcapt >= QLT_COUNT) update_rx_quality();
    }

    if( h80211[0] == 0x80 )
    {
        ap_cur->nb_bcn++;
    }

    ap_cur->nb_pkt++;

    /* find wpa handshake */
    if( h80211[0] == 0x10 )
    {
        /* reset the WPA handshake state */

        if( st_cur != NULL && st_cur->wpa.state != 0xFF )
            st_cur->wpa.state = 0;
//        printf("initial auth %d\n", ap_cur->wpa_state);
    }

    /* locate the station MAC in the 802.11 header */

    switch( h80211[1] & 3 )
    {
        case  0:

            /* if management, check that SA != BSSID */

            if( memcmp( h80211 + 10, bssid, 6 ) == 0 )
                goto skip_station;

            memcpy( stmac, h80211 + 10, 6 );
            break;

        case  1:

            /* ToDS packet, must come from a client */

            memcpy( stmac, h80211 + 10, 6 );
            break;

        case  2:

            /* FromDS packet, reject broadcast MACs */

            if( (h80211[4]%2) != 0 ) goto skip_station;
            memcpy( stmac, h80211 +  4, 6 ); break;

        default: goto skip_station;
    }

    /* update our chained list of wireless stations */

    st_cur = G.st_1st;
    st_prv = NULL;

    while( st_cur != NULL )
    {
        if( ! memcmp( st_cur->stmac, stmac, 6 ) )
            break;

        st_prv = st_cur;
        st_cur = st_cur->next;
    }

    /* if it's a new client, add it */

    if( st_cur == NULL )
    {
        if( ! ( st_cur = (struct ST_info *) malloc(
                         sizeof( struct ST_info ) ) ) )
        {
            perror( "malloc failed" );
            return( 1 );
        }

        /* if mac is listed as unknown, remove it */
        remove_namac(stmac);

        memset( st_cur, 0, sizeof( struct ST_info ) );

        if( G.st_1st == NULL )
            G.st_1st = st_cur;
        else
            st_prv->next  = st_cur;

        memcpy( st_cur->stmac, stmac, 6 );

		if (st_cur->manuf == NULL) {
			st_cur->manuf = get_manufacturer(st_cur->stmac[0], st_cur->stmac[1], st_cur->stmac[2]);
		}

        st_cur->prev = st_prv;

        st_cur->tinit = time( NULL );
        st_cur->tlast = time( NULL );

        st_cur->power = -1;
        st_cur->rate_to = -1;
        st_cur->rate_from = -1;

        st_cur->probe_index = -1;
        st_cur->missed  = 0;
        st_cur->lastseq = 0;
        st_cur->qos_fr_ds = 0;
        st_cur->qos_to_ds = 0;
        gettimeofday( &(st_cur->ftimer), NULL);

        for( i = 0; i < NB_PRB; i++ )
        {
            memset( st_cur->probes[i], 0, sizeof(
                    st_cur->probes[i] ) );
            st_cur->ssid_length[i] = 0;
        }

        G.st_end = st_cur;
    }

    if( st_cur->base == NULL ||
        memcmp( ap_cur->bssid, BROADCAST, 6 ) != 0 )
        st_cur->base = ap_cur;

    //update bitrate to station
    if( (st_cur != NULL) && ( h80211[1] & 3 ) == 2 )
        st_cur->rate_to = ri->ri_rate;

    /* update the last time seen */

    st_cur->tlast = time( NULL );

    /* only update power if packets comes from the
     * client: either type == Mgmt and SA != BSSID,
     * or FromDS == 0 and ToDS == 1 */

    if( ( ( h80211[1] & 3 ) == 0 &&
            memcmp( h80211 + 10, bssid, 6 ) != 0 ) ||
        ( ( h80211[1] & 3 ) == 1 ) )
    {
        st_cur->power = ri->ri_power;
        st_cur->rate_from = ri->ri_rate;

        if(st_cur->lastseq != 0)
        {
            msd = seq - st_cur->lastseq - 1;
            if(msd > 0 && msd < 1000)
                st_cur->missed += msd;
        }
        st_cur->lastseq = seq;
    }

    st_cur->nb_pkt++;

skip_station:

    /* packet parsing: Probe Request */

    if( h80211[0] == 0x40 && st_cur != NULL )
    {
        p = h80211 + 24;

        while( p < h80211 + caplen )
        {
            if( p + 2 + p[1] > h80211 + caplen )
                break;

            if( p[0] == 0x00 && p[1] > 0 && p[2] != '\0' &&
                ( p[1] > 1 || p[2] != ' ' ) )
            {
//                n = ( p[1] > 32 ) ? 32 : p[1];
                n = p[1];

                for( i = 0; i < n; i++ )
                    if( p[2 + i] > 0 && p[2 + i] < ' ' )
                        goto skip_probe;

                /* got a valid ASCII probed ESSID, check if it's
                   already in the ring buffer */

                for( i = 0; i < NB_PRB; i++ )
                    if( memcmp( st_cur->probes[i], p + 2, n ) == 0 )
                        goto skip_probe;

                st_cur->probe_index = ( st_cur->probe_index + 1 ) % NB_PRB;
                memset( st_cur->probes[st_cur->probe_index], 0, 256 );
                memcpy( st_cur->probes[st_cur->probe_index], p + 2, n ); //twice?!
                st_cur->ssid_length[st_cur->probe_index] = n;

                for( i = 0; i < n; i++ )
                {
                    c = p[2 + i];
                    if( c == 0 || ( c > 126 && c < 160 ) ) c = '.';  //could also check ||(c>0 && c<32)
                    st_cur->probes[st_cur->probe_index][i] = c;
                }
            }

            p += 2 + p[1];
        }
    }

skip_probe:

    /* packet parsing: Beacon or Probe Response */

    if( h80211[0] == 0x80 || h80211[0] == 0x50 )
    {
        if( !(ap_cur->security & (STD_OPN|STD_WEP|STD_WPA|STD_WPA2)) )
        {
            if( ( h80211[34] & 0x10 ) >> 4 ) ap_cur->security |= STD_WEP|ENC_WEP;
            else ap_cur->security |= STD_OPN;
        }

        ap_cur->preamble = ( h80211[34] & 0x20 ) >> 5;

        unsigned long long *tstamp = (unsigned long long *) (h80211 + 24);
        ap_cur->timestamp = letoh64(*tstamp);

        p = h80211 + 36;

        while( p < h80211 + caplen )
        {
            if( p + 2 + p[1] > h80211 + caplen )
                break;

            //only update the essid length if the new length is > the old one
            if( p[0] == 0x00 && (ap_cur->ssid_length < p[1]) ) ap_cur->ssid_length = p[1];

            if( p[0] == 0x00 && p[1] > 0 && p[2] != '\0' &&
                ( p[1] > 1 || p[2] != ' ' ) )
            {
                /* found a non-cloaked ESSID */

//                n = ( p[1] > 32 ) ? 32 : p[1];
                n = p[1];

                memset( ap_cur->essid, 0, 256 );
                memcpy( ap_cur->essid, p + 2, n );

                if( G.f_ivs != NULL && !ap_cur->essid_stored )
                {
                    memset(&ivs2, '\x00', sizeof(struct ivs2_pkthdr));
                    ivs2.flags |= IVS2_ESSID;
                    ivs2.len += ap_cur->ssid_length;

                    if( memcmp( G.prev_bssid, ap_cur->bssid, 6 ) != 0 )
                    {
                        ivs2.flags |= IVS2_BSSID;
                        ivs2.len += 6;
                        memcpy( G.prev_bssid, ap_cur->bssid,  6 );
                    }

                    /* write header */
                    if( fwrite( &ivs2, 1, sizeof(struct ivs2_pkthdr), G.f_ivs )
                        != (size_t) sizeof(struct ivs2_pkthdr) )
                    {
                        perror( "fwrite(IV header) failed" );
                        return( 1 );
                    }

                    /* write BSSID */
                    if(ivs2.flags & IVS2_BSSID)
                    {
                        if( fwrite( ap_cur->bssid, 1, 6, G.f_ivs )
                            != (size_t) 6 )
                        {
                            perror( "fwrite(IV bssid) failed" );
                            return( 1 );
                        }
                    }

                    /* write essid */
                    if( fwrite( ap_cur->essid, 1, ap_cur->ssid_length, G.f_ivs )
                        != (size_t) ap_cur->ssid_length )
                    {
                        perror( "fwrite(IV essid) failed" );
                        return( 1 );
                    }

                    ap_cur->essid_stored = 1;
                }

                for( i = 0; i < n; i++ )
                    if( ( ap_cur->essid[i] >   0 && ap_cur->essid[i] <  32 ) ||
                        ( ap_cur->essid[i] > 126 && ap_cur->essid[i] < 160 ) )
                        ap_cur->essid[i] = '.';
            }

            /* get the maximum speed in Mb and the AP's channel */

            if( p[0] == 0x01 || p[0] == 0x32 )
            {
                if(ap_cur->max_speed < ( p[1 + p[1]] & 0x7F ) / 2)
                    ap_cur->max_speed = ( p[1 + p[1]] & 0x7F ) / 2;
            }

            if( p[0] == 0x03 )
                ap_cur->channel = p[2];

            p += 2 + p[1];
        }
    }

    /* packet parsing: Beacon & Probe response */

    if( (h80211[0] == 0x80 || h80211[0] == 0x50) && caplen > 38)
    {
        p=h80211+36;         //ignore hdr + fixed params

        while( p < h80211 + caplen )
        {
            type = p[0];
            length = p[1];
            if(p+2+length > h80211 + caplen) {
/*                printf("error parsing tags! %p vs. %p (tag: %i, length: %i,position: %i)\n", (p+2+length), (h80211+caplen), type, length, (p-h80211));
                exit(1);*/
                break;
            }

            if( (type == 0xDD && (length >= 8) && (memcmp(p+2, "\x00\x50\xF2\x01\x01\x00", 6) == 0)) || (type == 0x30) )
            {
                ap_cur->security &= ~(STD_WEP|ENC_WEP|STD_WPA);

                org_p = p;
                offset = 0;

                if(type == 0xDD)
                {
                    //WPA defined in vendor specific tag -> WPA1 support
                    ap_cur->security |= STD_WPA;
                    offset = 4;
                }

                if(type == 0x30)
                {
                    ap_cur->security |= STD_WPA2;
                    offset = 0;
                }

                if(length < (18+offset))
                {
                    p += length+2;
                    continue;
                }

                if( p+9+offset > h80211+caplen )
                    break;
                numuni  = p[8+offset] + (p[9+offset]<<8);

                if( p+ (11+offset) + 4*numuni > h80211+caplen)
                    break;
                numauth = p[(10+offset) + 4*numuni] + (p[(11+offset) + 4*numuni]<<8);

                p += (10+offset);

                if(type != 0x30)
                {
                    if( p + (4*numuni) + (2+4*numauth) > h80211+caplen)
                        break;
                }
                else
                {
                    if( p + (4*numuni) + (2+4*numauth) + 2 > h80211+caplen)
                        break;
                }

                for(i=0; i<numuni; i++)
                {
                    switch(p[i*4+3])
                    {
                    case 0x01:
                        ap_cur->security |= ENC_WEP;
                        break;
                    case 0x02:
                        ap_cur->security |= ENC_TKIP;
                        break;
                    case 0x03:
                        ap_cur->security |= ENC_WRAP;
                        break;
                    case 0x04:
                        ap_cur->security |= ENC_CCMP;
                        break;
                    case 0x05:
                        ap_cur->security |= ENC_WEP104;
                        break;
                    default:
                        break;
                    }
                }

                p += 2+4*numuni;

                for(i=0; i<numauth; i++)
                {
                    switch(p[i*4+3])
                    {
                    case 0x01:
                        ap_cur->security |= AUTH_MGT;
                        break;
                    case 0x02:
                        ap_cur->security |= AUTH_PSK;
                        break;
                    default:
                        break;
                    }
                }

                p += 2+4*numauth;

                if( type == 0x30 ) p += 2;

                p = org_p + length+2;
            }
            else if( (type == 0xDD && (length >= 8) && (memcmp(p+2, "\x00\x50\xF2\x02\x01\x01", 6) == 0)))
            {
                ap_cur->security |= STD_QOS;
                p += length+2;
            }
            else p += length+2;
        }
    }

    /* packet parsing: Authentication Response */

    if( h80211[0] == 0xB0 && caplen >= 30)
    {
        if( ap_cur->security & STD_WEP )
        {
            //successful step 2 or 4 (coming from the AP)
            if(memcmp(h80211+28, "\x00\x00", 2) == 0 &&
                (h80211[26] == 0x02 || h80211[26] == 0x04))
            {
                ap_cur->security &= ~(AUTH_OPN | AUTH_PSK | AUTH_MGT);
                if(h80211[24] == 0x00) ap_cur->security |= AUTH_OPN;
                if(h80211[24] == 0x01) ap_cur->security |= AUTH_PSK;
            }
        }
    }

    /* packet parsing: Association Request */

    if( h80211[0] == 0x00 && caplen > 28 )
    {
        p = h80211 + 28;

        while( p < h80211 + caplen )
        {
            if( p + 2 + p[1] > h80211 + caplen )
                break;

            if( p[0] == 0x00 && p[1] > 0 && p[2] != '\0' &&
                ( p[1] > 1 || p[2] != ' ' ) )
            {
                /* found a non-cloaked ESSID */

                n = ( p[1] > 32 ) ? 32 : p[1];

                memset( ap_cur->essid, 0, 33 );
                memcpy( ap_cur->essid, p + 2, n );

                if( G.f_ivs != NULL && !ap_cur->essid_stored )
                {
                    memset(&ivs2, '\x00', sizeof(struct ivs2_pkthdr));
                    ivs2.flags |= IVS2_ESSID;
                    ivs2.len += ap_cur->ssid_length;

                    if( memcmp( G.prev_bssid, ap_cur->bssid, 6 ) != 0 )
                    {
                        ivs2.flags |= IVS2_BSSID;
                        ivs2.len += 6;
                        memcpy( G.prev_bssid, ap_cur->bssid,  6 );
                    }

                    /* write header */
                    if( fwrite( &ivs2, 1, sizeof(struct ivs2_pkthdr), G.f_ivs )
                        != (size_t) sizeof(struct ivs2_pkthdr) )
                    {
                        perror( "fwrite(IV header) failed" );
                        return( 1 );
                    }

                    /* write BSSID */
                    if(ivs2.flags & IVS2_BSSID)
                    {
                        if( fwrite( ap_cur->bssid, 1, 6, G.f_ivs )
                            != (size_t) 6 )
                        {
                            perror( "fwrite(IV bssid) failed" );
                            return( 1 );
                        }
                    }

                    /* write essid */
                    if( fwrite( ap_cur->essid, 1, ap_cur->ssid_length, G.f_ivs )
                        != (size_t) ap_cur->ssid_length )
                    {
                        perror( "fwrite(IV essid) failed" );
                        return( 1 );
                    }

                    ap_cur->essid_stored = 1;
                }

                for( i = 0; i < n; i++ )
                    if( ap_cur->essid[i] < 32 ||
                      ( ap_cur->essid[i] > 126 && ap_cur->essid[i] < 160 ) )
                        ap_cur->essid[i] = '.';
            }

            p += 2 + p[1];
        }
        if(st_cur != NULL)
            st_cur->wpa.state = 0;
    }

    /* packet parsing: some data */

    if( ( h80211[0] & 0x0C ) == 0x08 )
    {
        /* update the channel if we didn't get any beacon */

        if( ap_cur->channel == -1 )
        {
            if(ri->ri_channel > 0 && ri->ri_channel < 167)
                ap_cur->channel = ri->ri_channel;
            else
                ap_cur->channel = G.channel[cardnum];
        }

        /* check the SNAP header to see if data is encrypted */

        z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;

        /* Check if 802.11e (QoS) */
        if( (h80211[0] & 0x80) == 0x80)
        {
            z+=2;
            if(st_cur != NULL)
            {
                if( (h80211[1] & 3) == 1 ) //ToDS
                    st_cur->qos_to_ds = 1;
                else
                    st_cur->qos_fr_ds = 1;
            }
        }
        else
        {
            if(st_cur != NULL)
            {
                if( (h80211[1] & 3) == 1 ) //ToDS
                    st_cur->qos_to_ds = 0;
                else
                    st_cur->qos_fr_ds = 0;
            }
        }

        if(z==24)
        {
            if(list_check_decloak(&(ap_cur->packets), caplen, h80211) != 0)
            {
                list_add_packet(&(ap_cur->packets), caplen, h80211);
            }
            else
            {
                ap_cur->is_decloak = 1;
                ap_cur->decloak_detect = 0;
                list_tail_free(&(ap_cur->packets));
                memset(G.message, '\x00', sizeof(G.message));
                    snprintf( G.message, sizeof( G.message ) - 1,
                        "][ Decloak: %02X:%02X:%02X:%02X:%02X:%02X ",
                        ap_cur->bssid[0], ap_cur->bssid[1], ap_cur->bssid[2],
                        ap_cur->bssid[3], ap_cur->bssid[4], ap_cur->bssid[5]);
            }
        }

        if( z + 26 > (unsigned)caplen )
            goto write_packet;

        if( h80211[z] == h80211[z + 1] && h80211[z + 2] == 0x03 )
        {
//            if( ap_cur->encryption < 0 )
//                ap_cur->encryption = 0;

            /* if ethertype == IPv4, find the LAN address */

            if( h80211[z + 6] == 0x08 && h80211[z + 7] == 0x00 &&
                ( h80211[1] & 3 ) == 0x01 )
                    memcpy( ap_cur->lanip, &h80211[z + 20], 4 );

            if( h80211[z + 6] == 0x08 && h80211[z + 7] == 0x06 )
                memcpy( ap_cur->lanip, &h80211[z + 22], 4 );
        }
//        else
//            ap_cur->encryption = 2 + ( ( h80211[z + 3] & 0x20 ) >> 5 );


        if(ap_cur->security == 0 || (ap_cur->security & STD_WEP) )
        {
            if( (h80211[1] & 0x40) != 0x40 )
            {
                ap_cur->security |= STD_OPN;
            }
            else
            {
                if((h80211[z+3] & 0x20) == 0x20)
                {
                    ap_cur->security |= STD_WPA;
                }
                else
                {
                    ap_cur->security |= STD_WEP;
                    if( (h80211[z+3] & 0xC0) != 0x00)
                    {
                        ap_cur->security |= ENC_WEP40;
                    }
                    else
                    {
                        ap_cur->security &= ~ENC_WEP40;
                        ap_cur->security |= ENC_WEP;
                    }
                }
            }
        }

        if( z + 10 > (unsigned)caplen )
            goto write_packet;

        if( ap_cur->security & STD_WEP )
        {
            /* WEP: check if we've already seen this IV */

            if( ! uniqueiv_check( ap_cur->uiv_root, &h80211[z] ) )
            {
                /* first time seen IVs */

                if( G.f_ivs != NULL )
                {
                    memset(&ivs2, '\x00', sizeof(struct ivs2_pkthdr));
                    ivs2.flags = 0;
                    ivs2.len = 0;

                    /* datalen = caplen - (header+iv+ivs) */
                    dlen = caplen -z -4 -4; //original data len
                    if(dlen > 2048) dlen = 2048;
                    //get cleartext + len + 4(iv+idx)
                    num_xor = known_clear(clear, &clen, weight, h80211, dlen);
                    if(num_xor == 1)
                    {
                        ivs2.flags |= IVS2_XOR;
                        ivs2.len += clen + 4;
                        /* reveal keystream (plain^encrypted) */
                        for(n=0; n<(ivs2.len-4); n++)
                        {
                            clear[n] = (clear[n] ^ h80211[z+4+n]) & 0xFF;
                        }
                        //clear is now the keystream
                    }
                    else
                    {
                        //do it again to get it 2 bytes higher
                        num_xor = known_clear(clear+2, &clen, weight, h80211, dlen);
                        ivs2.flags |= IVS2_PTW;
                        //len = 4(iv+idx) + 1(num of keystreams) + 1(len per keystream) + 32*num_xor + 16*sizeof(int)(weight[16])
                        ivs2.len += 4 + 1 + 1 + 32*num_xor + 16*sizeof(int);
                        clear[0] = num_xor;
                        clear[1] = clen;
                        /* reveal keystream (plain^encrypted) */
                        for(o=0; o<num_xor; o++)
                        {
                            for(n=0; n<(ivs2.len-4); n++)
                            {
                                clear[2+n+o*32] = (clear[2+n+o*32] ^ h80211[z+4+n]) & 0xFF;
                            }
                        }
                        memcpy(clear+4 + 1 + 1 + 32*num_xor, weight, 16*sizeof(int));
                        //clear is now the keystream
                    }

                    if( memcmp( G.prev_bssid, ap_cur->bssid, 6 ) != 0 )
                    {
                        ivs2.flags |= IVS2_BSSID;
                        ivs2.len += 6;
                        memcpy( G.prev_bssid, ap_cur->bssid,  6 );
                    }

                    if( fwrite( &ivs2, 1, sizeof(struct ivs2_pkthdr), G.f_ivs )
                        != (size_t) sizeof(struct ivs2_pkthdr) )
                    {
                        perror( "fwrite(IV header) failed" );
                        return( 1 );
                    }

                    if( ivs2.flags & IVS2_BSSID )
                    {
                        if( fwrite( ap_cur->bssid, 1, 6, G.f_ivs ) != (size_t) 6 )
                        {
                            perror( "fwrite(IV bssid) failed" );
                            return( 1 );
                        }
                        ivs2.len -= 6;
                    }

                    if( fwrite( h80211+z, 1, 4, G.f_ivs ) != (size_t) 4 )
                    {
                        perror( "fwrite(IV iv+idx) failed" );
                        return( 1 );
                    }
                    ivs2.len -= 4;

                    if( fwrite( clear, 1, ivs2.len, G.f_ivs ) != (size_t) ivs2.len )
                    {
                        perror( "fwrite(IV keystream) failed" );
                        return( 1 );
                    }
                }

                uniqueiv_mark( ap_cur->uiv_root, &h80211[z] );

                ap_cur->nb_data++;
            }

            // Record all data linked to IV to detect WEP Cloaking
            if( G.f_ivs == NULL && G.detect_anomaly)
            {
				// Only allocate this when seeing WEP AP
				if (ap_cur->data_root == NULL)
					ap_cur->data_root = data_init();

				// Only works with full capture, not IV-only captures
				if (data_check(ap_cur->data_root, &h80211[z], &h80211[z + 4])
					== CLOAKING && ap_cur->EAP_detected == 0)
				{

					//If no EAP/EAP was detected, indicate WEP cloaking
                    memset(G.message, '\x00', sizeof(G.message));
                    snprintf( G.message, sizeof( G.message ) - 1,
                        "][ WEP Cloaking: %02X:%02X:%02X:%02X:%02X:%02X ",
                        ap_cur->bssid[0], ap_cur->bssid[1], ap_cur->bssid[2],
                        ap_cur->bssid[3], ap_cur->bssid[4], ap_cur->bssid[5]);

				}
			}

        }
        else
        {
            ap_cur->nb_data++;
        }

        z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;

        /* Check if 802.11e (QoS) */
        if( (h80211[0] & 0x80) == 0x80) z+=2;

        if( z + 26 > (unsigned)caplen )
            goto write_packet;

        z += 6;     //skip LLC header

        /* check ethertype == EAPOL */
        if( h80211[z] == 0x88 && h80211[z + 1] == 0x8E && (h80211[1] & 0x40) != 0x40 )
        {
			ap_cur->EAP_detected = 1;

            z += 2;     //skip ethertype

            if( st_cur == NULL )
                goto write_packet;

            /* frame 1: Pairwise == 1, Install == 0, Ack == 1, MIC == 0 */

            if( ( h80211[z + 6] & 0x08 ) != 0 &&
                  ( h80211[z + 6] & 0x40 ) == 0 &&
                  ( h80211[z + 6] & 0x80 ) != 0 &&
                  ( h80211[z + 5] & 0x01 ) == 0 )
            {
                memcpy( st_cur->wpa.anonce, &h80211[z + 17], 32 );
                st_cur->wpa.state = 1;
            }


            /* frame 2 or 4: Pairwise == 1, Install == 0, Ack == 0, MIC == 1 */

            if( z+17+32 > (unsigned)caplen )
                goto write_packet;

            if( ( h80211[z + 6] & 0x08 ) != 0 &&
                  ( h80211[z + 6] & 0x40 ) == 0 &&
                  ( h80211[z + 6] & 0x80 ) == 0 &&
                  ( h80211[z + 5] & 0x01 ) != 0 )
            {
                if( memcmp( &h80211[z + 17], ZERO, 32 ) != 0 )
                {
                    memcpy( st_cur->wpa.snonce, &h80211[z + 17], 32 );
                    st_cur->wpa.state |= 2;

                }

                if( (st_cur->wpa.state & 4) != 4 )
                {
                    st_cur->wpa.eapol_size = ( h80211[z + 2] << 8 )
                            +   h80211[z + 3] + 4;

                    if (caplen - z < st_cur->wpa.eapol_size || st_cur->wpa.eapol_size == 0 ||
                        caplen - z < 81 + 16 || st_cur->wpa.eapol_size > sizeof(st_cur->wpa.eapol))
                    {
                        // Ignore the packet trying to crash us.
                        st_cur->wpa.eapol_size = 0;
                        goto write_packet;
                    }

                    memcpy( st_cur->wpa.keymic, &h80211[z + 81], 16 );
                    memcpy( st_cur->wpa.eapol,  &h80211[z], st_cur->wpa.eapol_size );
                    memset( st_cur->wpa.eapol + 81, 0, 16 );
                    st_cur->wpa.state |= 4;
                    st_cur->wpa.keyver = h80211[z + 6] & 7;
                }
            }

            /* frame 3: Pairwise == 1, Install == 1, Ack == 1, MIC == 1 */

            if( ( h80211[z + 6] & 0x08 ) != 0 &&
                  ( h80211[z + 6] & 0x40 ) != 0 &&
                  ( h80211[z + 6] & 0x80 ) != 0 &&
                  ( h80211[z + 5] & 0x01 ) != 0 )
            {
                if( memcmp( &h80211[z + 17], ZERO, 32 ) != 0 )
                {
                    memcpy( st_cur->wpa.anonce, &h80211[z + 17], 32 );
                    st_cur->wpa.state |= 1;
                }

                if( (st_cur->wpa.state & 4) != 4 )
                {
                    st_cur->wpa.eapol_size = ( h80211[z + 2] << 8 )
                            +   h80211[z + 3] + 4;

                    if (caplen - (unsigned)z < st_cur->wpa.eapol_size || st_cur->wpa.eapol_size == 0 ||
                        caplen - (unsigned)z < 81 + 16 || st_cur->wpa.eapol_size > sizeof(st_cur->wpa.eapol))
                    {
                        // Ignore the packet trying to crash us.
                        st_cur->wpa.eapol_size = 0;
                        goto write_packet;
                    }

                    memcpy( st_cur->wpa.keymic, &h80211[z + 81], 16 );
                    memcpy( st_cur->wpa.eapol,  &h80211[z], st_cur->wpa.eapol_size );
                    memset( st_cur->wpa.eapol + 81, 0, 16 );
                    st_cur->wpa.state |= 4;
                    st_cur->wpa.keyver = h80211[z + 6] & 7;
                }
            }

            if( st_cur->wpa.state == 7)
            {
                memcpy( st_cur->wpa.stmac, st_cur->stmac, 6 );
                memcpy( G.wpa_bssid, ap_cur->bssid, 6 );
                memset(G.message, '\x00', sizeof(G.message));
                snprintf( G.message, sizeof( G.message ) - 1,
                    "][ WPA handshake: %02X:%02X:%02X:%02X:%02X:%02X ",
                    G.wpa_bssid[0], G.wpa_bssid[1], G.wpa_bssid[2],
                    G.wpa_bssid[3], G.wpa_bssid[4], G.wpa_bssid[5]);


                if( G.f_ivs != NULL )
                {
                    memset(&ivs2, '\x00', sizeof(struct ivs2_pkthdr));
                    ivs2.flags = 0;
                    ivs2.len = 0;

                    ivs2.len= sizeof(struct WPA_hdsk);
                    ivs2.flags |= IVS2_WPA;

                    if( memcmp( G.prev_bssid, ap_cur->bssid, 6 ) != 0 )
                    {
                        ivs2.flags |= IVS2_BSSID;
                        ivs2.len += 6;
                        memcpy( G.prev_bssid, ap_cur->bssid,  6 );
                    }

                    if( fwrite( &ivs2, 1, sizeof(struct ivs2_pkthdr), G.f_ivs )
                        != (size_t) sizeof(struct ivs2_pkthdr) )
                    {
                        perror( "fwrite(IV header) failed" );
                        return( 1 );
                    }

                    if( ivs2.flags & IVS2_BSSID )
                    {
                        if( fwrite( ap_cur->bssid, 1, 6, G.f_ivs ) != (size_t) 6 )
                        {
                            perror( "fwrite(IV bssid) failed" );
                            return( 1 );
                        }
                        ivs2.len -= 6;
                    }

                    if( fwrite( &(st_cur->wpa), 1, sizeof(struct WPA_hdsk), G.f_ivs ) != (size_t) sizeof(struct WPA_hdsk) )
                    {
                        perror( "fwrite(IV wpa_hdsk) failed" );
                        return( 1 );
                    }
                }
            }
        }
    }


write_packet:

    if(ap_cur != NULL)
    {
        if( h80211[0] == 0x80 && G.one_beacon){
            if( !ap_cur->beacon_logged )
                ap_cur->beacon_logged = 1;
            else return ( 0 );
        }
    }

    if(G.record_data)
    {
        if( ( (h80211[0] & 0x0C) == 0x00 ) && ( (h80211[0] & 0xF0) == 0xB0 ) )
        {
            /* authentication packet */
            check_shared_key(h80211, caplen);
        }
    }

    if(ap_cur != NULL)
    {
        if(ap_cur->security != 0 && G.f_encrypt != 0 && ((ap_cur->security & G.f_encrypt) == 0))
        {
            return(1);
        }

        if(is_filtered_essid(ap_cur->essid))
        {
            return(1);
        }

    }

    /* this changes the local ap_cur, st_cur and na_cur variables and should be the last check befor the actual write */
    if(caplen < 24 && caplen >= 10 && h80211[0])
    {
        /* RTS || CTS || ACK || CF-END || CF-END&CF-ACK*/
        //(h80211[0] == 0xB4 || h80211[0] == 0xC4 || h80211[0] == 0xD4 || h80211[0] == 0xE4 || h80211[0] == 0xF4)

        /* use general control frame detection, as the structure is always the same: mac(s) starting at [4] */
        if(h80211[0] & 0x04)
        {
            p=h80211+4;
            while(p <= h80211+16 && p<=h80211+caplen)
            {
                memcpy(namac, p, 6);

                if(memcmp(namac, NULL_MAC, 6) == 0)
                {
                    p+=6;
                    continue;
                }

                if(memcmp(namac, BROADCAST, 6) == 0)
                {
                    p+=6;
                    continue;
                }

                if(G.hide_known)
                {
                    /* check AP list */
                    ap_cur = G.ap_1st;
                    ap_prv = NULL;

                    while( ap_cur != NULL )
                    {
                        if( ! memcmp( ap_cur->bssid, namac, 6 ) )
                            break;

                        ap_prv = ap_cur;
                        ap_cur = ap_cur->next;
                    }

                    /* if it's an AP, try next mac */

                    if( ap_cur != NULL )
                    {
                        p+=6;
                        continue;
                    }

                    /* check ST list */
                    st_cur = G.st_1st;
                    st_prv = NULL;

                    while( st_cur != NULL )
                    {
                        if( ! memcmp( st_cur->stmac, namac, 6 ) )
                            break;

                        st_prv = st_cur;
                        st_cur = st_cur->next;
                    }

                    /* if it's a client, try next mac */

                    if( st_cur != NULL )
                    {
                        p+=6;
                        continue;
                    }
                }

                /* not found in either AP list or ST list, look through NA list */
                na_cur = G.na_1st;
                na_prv = NULL;

                while( na_cur != NULL )
                {
                    if( ! memcmp( na_cur->namac, namac, 6 ) )
                        break;

                    na_prv = na_cur;
                    na_cur = na_cur->next;
                }

                /* update our chained list of unknown stations */
                /* if it's a new mac, add it */

                if( na_cur == NULL )
                {
                    if( ! ( na_cur = (struct NA_info *) malloc(
                                    sizeof( struct NA_info ) ) ) )
                    {
                        perror( "malloc failed" );
                        return( 1 );
                    }

                    memset( na_cur, 0, sizeof( struct NA_info ) );

                    if( G.na_1st == NULL )
                        G.na_1st = na_cur;
                    else
                        na_prv->next  = na_cur;

                    memcpy( na_cur->namac, namac, 6 );

                    na_cur->prev = na_prv;

                    gettimeofday(&(na_cur->tv), NULL);
                    na_cur->tinit = time( NULL );
                    na_cur->tlast = time( NULL );

                    na_cur->power   = -1;
                    na_cur->channel = -1;
                    na_cur->ack     = 0;
                    na_cur->ack_old = 0;
                    na_cur->ackps   = 0;
                    na_cur->cts     = 0;
                    na_cur->rts_r   = 0;
                    na_cur->rts_t   = 0;
                }

                /* update the last time seen & power*/

                na_cur->tlast = time( NULL );
                na_cur->power = ri->ri_power;
                na_cur->channel = ri->ri_channel;

                switch(h80211[0] & 0xF0)
                {
                    case 0xB0:
                        if(p == h80211+4)
                            na_cur->rts_r++;
                        if(p == h80211+10)
                            na_cur->rts_t++;
                        break;

                    case 0xC0:
                        na_cur->cts++;
                        break;

                    case 0xD0:
                        na_cur->ack++;
                        break;

                    default:
                        na_cur->other++;
                        break;
                }

                /*grab next mac (for rts frames)*/
                p+=6;
            }
        }
    }

    if( G.f_cap != NULL && caplen >= 10)
    {
        pkh.caplen = pkh.len = caplen;

        gettimeofday( &tv, NULL );

        pkh.tv_sec  =   tv.tv_sec;
        pkh.tv_usec = ( tv.tv_usec & ~0x1ff ) + ri->ri_power + 64;

        n = sizeof( pkh );

        if( fwrite( &pkh, 1, n, G.f_cap ) != (size_t) n )
        {
            perror( "fwrite(packet header) failed" );
            return( 1 );
        }

        fflush( stdout );

        n = pkh.caplen;

        if( fwrite( h80211, 1, n, G.f_cap ) != (size_t) n )
        {
            perror( "fwrite(packet data) failed" );
            return( 1 );
        }

        fflush( stdout );
    }

    return( 0 );
}