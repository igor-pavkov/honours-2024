void dump_print( int ws_row, int ws_col, int if_num )
{
    time_t tt;
    struct tm *lt;
    int nlines, i, n, len;
    char strbuf[512];
    char buffer[512];
    char ssid_list[512];
    struct AP_info *ap_cur;
    struct ST_info *st_cur;
    struct NA_info *na_cur;
    int columns_ap = 83;
    int columns_sta = 74;
    int columns_na = 68;

    int num_ap;
    int num_sta;

    if(!G.singlechan) columns_ap -= 4; //no RXQ in scan mode
    if(G.show_uptime) columns_ap += 15; //show uptime needs more space

    nlines = 2;

    if( nlines >= ws_row )
        return;

    if(G.do_sort_always) {
	pthread_mutex_lock( &(G.mx_sort) );
	    dump_sort();
	pthread_mutex_unlock( &(G.mx_sort) );
    }

    tt = time( NULL );
    lt = localtime( &tt );

    if(G.is_berlin)
    {
        G.maxaps = 0;
        G.numaps = 0;
        ap_cur = G.ap_end;

        while( ap_cur != NULL )
        {
            G.maxaps++;
            if( ap_cur->nb_pkt < 2 || time( NULL ) - ap_cur->tlast > G.berlin ||
                memcmp( ap_cur->bssid, BROADCAST, 6 ) == 0 )
            {
                ap_cur = ap_cur->prev;
                continue;
            }
            G.numaps++;
            ap_cur = ap_cur->prev;
        }

        if(G.numaps > G.maxnumaps)
            G.maxnumaps = G.numaps;

//        G.maxaps--;
    }

    /*
     *  display the channel, battery, position (if we are connected to GPSd)
     *  and current time
     */

    memset( strbuf, '\0', sizeof(strbuf) );
    strbuf[ws_col - 1] = '\0';
    fprintf( stderr, "%s\n", strbuf );

    if(G.freqoption)
    {
        snprintf(strbuf, sizeof(strbuf)-1, " Freq %4d", G.frequency[0]);
        for(i=1; i<if_num; i++)
        {
            memset( buffer, '\0', sizeof(buffer) );
            snprintf(buffer, sizeof(buffer) , ",%4d", G.frequency[i]);
            strncat(strbuf, buffer, sizeof(strbuf) - strlen(strbuf) - 1);
        }
    }
    else
    {
        snprintf(strbuf, sizeof(strbuf)-1, " CH %2d", G.channel[0]);
        for(i=1; i<if_num; i++)
        {
            memset( buffer, '\0', sizeof(buffer) );
            snprintf(buffer, sizeof(buffer) , ",%2d", G.channel[i]);
            strncat(strbuf, buffer, sizeof(strbuf) - strlen(strbuf) -1);
        }
    }
    memset( buffer, '\0', sizeof(buffer) );

    if (G.gps_loc[0]) {
        snprintf( buffer, sizeof( buffer ) - 1,
              " %s[ GPS %8.3f %8.3f %8.3f %6.2f "
              "][ Elapsed: %s ][ %04d-%02d-%02d %02d:%02d ", G.batt,
              G.gps_loc[0], G.gps_loc[1], G.gps_loc[2], G.gps_loc[3],
              G.elapsed_time , 1900 + lt->tm_year,
              1 + lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min );
    }
    else
    {
        snprintf( buffer, sizeof( buffer ) - 1,
              " %s[ Elapsed: %s ][ %04d-%02d-%02d %02d:%02d ",
              G.batt, G.elapsed_time, 1900 + lt->tm_year,
              1 + lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min );
    }

    strncat(strbuf, buffer, (512-strlen(strbuf)));
    memset( buffer, '\0', 512 );

    if(G.is_berlin)
    {
        snprintf( buffer, sizeof( buffer ) - 1,
              " ][%3d/%3d/%4d ",
              G.numaps, G.maxnumaps, G.maxaps);
    }

    strncat(strbuf, buffer, (512-strlen(strbuf)));
    memset( buffer, '\0', 512 );

    if(strlen(G.message) > 0)
    {
        strncat(strbuf, G.message, (512-strlen(strbuf)));
    }

    //add traling spaces to overwrite previous messages
    strncat(strbuf, "                                        ", (512-strlen(strbuf)));

    strbuf[ws_col - 1] = '\0';
    fprintf( stderr, "%s\n", strbuf );

    /* print some informations about each detected AP */

    nlines += 3;

    if( nlines >= ws_row )
        return;

    memset( strbuf, ' ', ws_col - 1 );
    strbuf[ws_col - 1] = '\0';
    fprintf( stderr, "%s\n", strbuf );

    if(G.show_ap) {

    strbuf[0] = 0;
    strcat(strbuf, " BSSID              PWR ");

    if(G.singlechan)
    	strcat(strbuf, "RXQ ");

    strcat(strbuf, " Beacons    #Data, #/s  CH  MB   ENC  CIPHER AUTH ");

    if (G.show_uptime)
    	strcat(strbuf, "       UPTIME  ");

    strcat(strbuf, "ESSID");

	if ( G.show_manufacturer && ( ws_col > (columns_ap - 4) ) ) {
		// write spaces (32).
		memset(strbuf+columns_ap, 32, G.maxsize_essid_seen - 5 ); // 5 is the len of "ESSID"
		snprintf(strbuf+columns_ap+G.maxsize_essid_seen-5, 15,"%s","  MANUFACTURER");
	}

	strbuf[ws_col - 1] = '\0';
	fprintf( stderr, "%s\n", strbuf );

	memset( strbuf, ' ', ws_col - 1 );
	strbuf[ws_col - 1] = '\0';
	fprintf( stderr, "%s\n", strbuf );

	ap_cur = G.ap_end;

	if(G.selection_ap) {
	    num_ap = get_ap_list_count();
	    if(G.selected_ap > num_ap)
		G.selected_ap = num_ap;
	}

	if(G.selection_sta) {
	    num_sta = get_sta_list_count();
	    if(G.selected_sta > num_sta)
		G.selected_sta = num_sta;
	}

	num_ap = 0;

	if(G.selection_ap) {
	    G.start_print_ap = G.selected_ap - ((ws_row-1) - nlines) + 1;
	    if(G.start_print_ap < 1)
		G.start_print_ap = 1;
    //	printf("%i\n", G.start_print_ap);
	}


	while( ap_cur != NULL )
	{
	    /* skip APs with only one packet, or those older than 2 min.
	    * always skip if bssid == broadcast */

	    if( ap_cur->nb_pkt < 2 || time( NULL ) - ap_cur->tlast > G.berlin ||
		memcmp( ap_cur->bssid, BROADCAST, 6 ) == 0 )
	    {
		ap_cur = ap_cur->prev;
		continue;
	    }

	    if(ap_cur->security != 0 && G.f_encrypt != 0 && ((ap_cur->security & G.f_encrypt) == 0))
	    {
		ap_cur = ap_cur->prev;
		continue;
	    }

	    if(is_filtered_essid(ap_cur->essid))
	    {
		ap_cur = ap_cur->prev;
		continue;
	    }

	    num_ap++;

	    if(num_ap < G.start_print_ap) {
		ap_cur = ap_cur->prev;
		continue;
	    }

	    nlines++;

	    if( nlines > (ws_row-1) )
		return;

	    memset(strbuf, '\0', sizeof(strbuf));

	    snprintf( strbuf, sizeof(strbuf), " %02X:%02X:%02X:%02X:%02X:%02X",
		    ap_cur->bssid[0], ap_cur->bssid[1],
		    ap_cur->bssid[2], ap_cur->bssid[3],
		    ap_cur->bssid[4], ap_cur->bssid[5] );

	    len = strlen(strbuf);

	    if(G.singlechan)
	    {
		snprintf( strbuf+len, sizeof(strbuf)-len, "  %3d %3d %8ld %8ld %4d",
			ap_cur->avg_power,
			ap_cur->rx_quality,
			ap_cur->nb_bcn,
			ap_cur->nb_data,
			ap_cur->nb_dataps );
	    }
	    else
	    {
		snprintf( strbuf+len, sizeof(strbuf)-len, "  %3d %8ld %8ld %4d",
			ap_cur->avg_power,
			ap_cur->nb_bcn,
			ap_cur->nb_data,
			ap_cur->nb_dataps );
	    }

	    len = strlen(strbuf);

	    snprintf( strbuf+len, sizeof(strbuf)-len, " %3d %3d%c%c ",
		    ap_cur->channel, ap_cur->max_speed,
		    ( ap_cur->security & STD_QOS ) ? 'e' : ' ',
		    ( ap_cur->preamble ) ? '.' : ' ');

	    len = strlen(strbuf);

	    if( (ap_cur->security & (STD_OPN|STD_WEP|STD_WPA|STD_WPA2)) == 0) snprintf( strbuf+len, sizeof(strbuf)-len, "    " );
	    else if( ap_cur->security & STD_WPA2 ) snprintf( strbuf+len, sizeof(strbuf)-len, "WPA2" );
	    else if( ap_cur->security & STD_WPA  ) snprintf( strbuf+len, sizeof(strbuf)-len, "WPA " );
	    else if( ap_cur->security & STD_WEP  ) snprintf( strbuf+len, sizeof(strbuf)-len, "WEP " );
	    else if( ap_cur->security & STD_OPN  ) snprintf( strbuf+len, sizeof(strbuf)-len, "OPN " );

	    strncat( strbuf, " ", sizeof(strbuf) - strlen(strbuf) - 1);

	    len = strlen(strbuf);

	    if( (ap_cur->security & (ENC_WEP|ENC_TKIP|ENC_WRAP|ENC_CCMP|ENC_WEP104|ENC_WEP40)) == 0 ) snprintf( strbuf+len, sizeof(strbuf)-len, "       ");
	    else if( ap_cur->security & ENC_CCMP   ) snprintf( strbuf+len, sizeof(strbuf)-len, "CCMP   ");
	    else if( ap_cur->security & ENC_WRAP   ) snprintf( strbuf+len, sizeof(strbuf)-len, "WRAP   ");
	    else if( ap_cur->security & ENC_TKIP   ) snprintf( strbuf+len, sizeof(strbuf)-len, "TKIP   ");
	    else if( ap_cur->security & ENC_WEP104 ) snprintf( strbuf+len, sizeof(strbuf)-len, "WEP104 ");
	    else if( ap_cur->security & ENC_WEP40  ) snprintf( strbuf+len, sizeof(strbuf)-len, "WEP40  ");
	    else if( ap_cur->security & ENC_WEP    ) snprintf( strbuf+len, sizeof(strbuf)-len, "WEP    ");

	    len = strlen(strbuf);

	    if( (ap_cur->security & (AUTH_OPN|AUTH_PSK|AUTH_MGT)) == 0 ) snprintf( strbuf+len, sizeof(strbuf)-len, "   ");
	    else if( ap_cur->security & AUTH_MGT   ) snprintf( strbuf+len, sizeof(strbuf)-len, "MGT");
	    else if( ap_cur->security & AUTH_PSK   )
	    {
		if( ap_cur->security & STD_WEP )
		    snprintf( strbuf+len, sizeof(strbuf)-len, "SKA");
		else
		    snprintf( strbuf+len, sizeof(strbuf)-len, "PSK");
	    }
	    else if( ap_cur->security & AUTH_OPN   ) snprintf( strbuf+len, sizeof(strbuf)-len, "OPN");

	    len = strlen(strbuf);

	    if (G.show_uptime) {
	    	snprintf(strbuf+len, sizeof(strbuf)-len, " %14s", parse_timestamp(ap_cur->timestamp));
	    	len = strlen(strbuf);
	    }

	    strbuf[ws_col-1] = '\0';

	    if(G.selection_ap && ((num_ap) == G.selected_ap)) {
		if(G.mark_cur_ap) {
		    if(ap_cur->marked == 0) {
			ap_cur->marked = 1;
		    }
		    else {
			ap_cur->marked_color++;
			if(ap_cur->marked_color > (TEXT_MAX_COLOR-1)) {
			    ap_cur->marked_color = 1;
			    ap_cur->marked = 0;
			}
		    }
		    G.mark_cur_ap = 0;
		}
		textstyle(TEXT_REVERSE);
		memcpy(G.selected_bssid, ap_cur->bssid, 6);
	    }

	    if(ap_cur->marked) {
		textcolor_fg(ap_cur->marked_color);
	    }

	    fprintf(stderr, "%s", strbuf);

	    if( ws_col > (columns_ap - 4) )
	    {
		memset( strbuf, 0, sizeof( strbuf ) );
		if(ap_cur->essid[0] != 0x00)
		{
		    snprintf( strbuf,  sizeof( strbuf ) - 1,
			    "%s", ap_cur->essid );
		}
		else
		{
		    snprintf( strbuf,  sizeof( strbuf ) - 1,
			    "<length:%3d>%s", ap_cur->ssid_length, "\x00" );
		}

		if (G.show_manufacturer) {

			if (G.maxsize_essid_seen <= strlen(strbuf))
				G.maxsize_essid_seen = strlen(strbuf);
			else // write spaces (32)
				memset( strbuf+strlen(strbuf), 32,  (G.maxsize_essid_seen - strlen(strbuf))  );

			if (ap_cur->manuf == NULL)
				ap_cur->manuf = get_manufacturer(ap_cur->bssid[0], ap_cur->bssid[1], ap_cur->bssid[2]);

			snprintf( strbuf + G.maxsize_essid_seen , sizeof(strbuf)-G.maxsize_essid_seen, "  %s", ap_cur->manuf );
		}

		// write spaces (32) until the end of column
		memset( strbuf+strlen(strbuf), 32, ws_col - (columns_ap - 4 ) );

		// end the string at the end of the column
		strbuf[ws_col - (columns_ap - 4)] = '\0';

		fprintf( stderr, "  %s", strbuf );
	    }

	    fprintf( stderr, "\n" );

	    if( (G.selection_ap && ((num_ap) == G.selected_ap)) || (ap_cur->marked) ) {
		textstyle(TEXT_RESET);
	    }

	    ap_cur = ap_cur->prev;
	}

	/* print some informations about each detected station */

	nlines += 3;

	if( nlines >= (ws_row-1) )
	    return;

	memset( strbuf, ' ', ws_col - 1 );
	strbuf[ws_col - 1] = '\0';
	fprintf( stderr, "%s\n", strbuf );
    }

    if(G.show_sta) {
	memcpy( strbuf, " BSSID              STATION "
		"           PWR   Rate    Lost    Frames  Probes", columns_sta );
	strbuf[ws_col - 1] = '\0';
	fprintf( stderr, "%s\n", strbuf );

	memset( strbuf, ' ', ws_col - 1 );
	strbuf[ws_col - 1] = '\0';
	fprintf( stderr, "%s\n", strbuf );

	ap_cur = G.ap_end;

	num_sta = 0;

	while( ap_cur != NULL )
	{
	    if( ap_cur->nb_pkt < 2 ||
		time( NULL ) - ap_cur->tlast > G.berlin )
	    {
		ap_cur = ap_cur->prev;
		continue;
	    }

	    if(ap_cur->security != 0 && G.f_encrypt != 0 && ((ap_cur->security & G.f_encrypt) == 0))
	    {
		ap_cur = ap_cur->prev;
		continue;
	    }

	    // Don't filter unassociated clients by ESSID
	    if(memcmp(ap_cur->bssid, BROADCAST, 6) && is_filtered_essid(ap_cur->essid))
	    {
		ap_cur = ap_cur->prev;
		continue;
	    }

	    if( nlines >= (ws_row-1) )
		return;

	    st_cur = G.st_end;

	    if(G.selection_ap && (memcmp(G.selected_bssid, ap_cur->bssid, 6)==0)) {
		textstyle(TEXT_REVERSE);
	    }

	    if(ap_cur->marked) {
		textcolor_fg(ap_cur->marked_color);
	    }

	    while( st_cur != NULL )
	    {
		if( st_cur->base != ap_cur ||
		    time( NULL ) - st_cur->tlast > G.berlin )
		{
		    st_cur = st_cur->prev;
		    continue;
		}

		if( ! memcmp( ap_cur->bssid, BROADCAST, 6 ) && G.asso_client )
		{
		    st_cur = st_cur->prev;
		    continue;
		}

		num_sta++;

		if(G.start_print_sta > num_sta)
		    continue;

		nlines++;

		if( ws_row != 0 && nlines >= ws_row )
		    return;

		if( ! memcmp( ap_cur->bssid, BROADCAST, 6 ) )
		    fprintf( stderr, " (not associated) " );
		else
		    fprintf( stderr, " %02X:%02X:%02X:%02X:%02X:%02X",
			    ap_cur->bssid[0], ap_cur->bssid[1],
			    ap_cur->bssid[2], ap_cur->bssid[3],
			    ap_cur->bssid[4], ap_cur->bssid[5] );

		fprintf( stderr, "  %02X:%02X:%02X:%02X:%02X:%02X",
			st_cur->stmac[0], st_cur->stmac[1],
			st_cur->stmac[2], st_cur->stmac[3],
			st_cur->stmac[4], st_cur->stmac[5] );

		fprintf( stderr, "  %3d ", st_cur->power    );
		fprintf( stderr, "  %2d", st_cur->rate_to/1000000  );
		fprintf( stderr,  "%c", (st_cur->qos_fr_ds) ? 'e' : ' ');
		fprintf( stderr,  "-%2d", st_cur->rate_from/1000000);
		fprintf( stderr,  "%c", (st_cur->qos_to_ds) ? 'e' : ' ');
		fprintf( stderr, "  %4d", st_cur->missed   );
		fprintf( stderr, " %8ld", st_cur->nb_pkt   );

		if( ws_col > (columns_sta - 6) )
		{
		    memset( ssid_list, 0, sizeof( ssid_list ) );

		    for( i = 0, n = 0; i < NB_PRB; i++ )
		    {
			if( st_cur->probes[i][0] == '\0' )
			    continue;

			snprintf( ssid_list + n, sizeof( ssid_list ) - n - 1,
				"%c%s", ( i > 0 ) ? ',' : ' ',
				st_cur->probes[i] );

			n += ( 1 + strlen( st_cur->probes[i] ) );

			if( n >= (int) sizeof( ssid_list ) )
			    break;
		    }

		    memset( strbuf, 0, sizeof( strbuf ) );
		    snprintf( strbuf,  sizeof( strbuf ) - 1,
			    "%-256s", ssid_list );
		    strbuf[ws_col - (columns_sta - 6)] = '\0';
		    fprintf( stderr, " %s", strbuf );
		}

		fprintf( stderr, "\n" );

		st_cur = st_cur->prev;
	    }

	    if( (G.selection_ap && (memcmp(G.selected_bssid, ap_cur->bssid, 6)==0)) || (ap_cur->marked) ) {
		textstyle(TEXT_RESET);
	    }

	    ap_cur = ap_cur->prev;
	}
    }

    if(G.show_ack)
    {
        /* print some informations about each unknown station */

        nlines += 3;

        if( nlines >= (ws_row-1) )
            return;

        memset( strbuf, ' ', ws_col - 1 );
        strbuf[ws_col - 1] = '\0';
        fprintf( stderr, "%s\n", strbuf );

        memcpy( strbuf, " MAC       "
                "          CH PWR    ACK ACK/s    CTS RTS_RX RTS_TX  OTHER", columns_na );
        strbuf[ws_col - 1] = '\0';
        fprintf( stderr, "%s\n", strbuf );

        memset( strbuf, ' ', ws_col - 1 );
        strbuf[ws_col - 1] = '\0';
        fprintf( stderr, "%s\n", strbuf );

        na_cur = G.na_1st;

        while( na_cur != NULL )
        {
            if( time( NULL ) - na_cur->tlast > 120 )
            {
                na_cur = na_cur->next;
                continue;
            }

            if( nlines >= (ws_row-1) )
                return;

            nlines++;

            if( ws_row != 0 && nlines >= ws_row )
                return;

            fprintf( stderr, " %02X:%02X:%02X:%02X:%02X:%02X",
                    na_cur->namac[0], na_cur->namac[1],
                    na_cur->namac[2], na_cur->namac[3],
                    na_cur->namac[4], na_cur->namac[5] );

            fprintf( stderr, "  %3d", na_cur->channel  );
            fprintf( stderr, " %3d", na_cur->power  );
            fprintf( stderr, " %6d", na_cur->ack );
            fprintf( stderr, "  %4d", na_cur->ackps );
            fprintf( stderr, " %6d", na_cur->cts );
            fprintf( stderr, " %6d", na_cur->rts_r );
            fprintf( stderr, " %6d", na_cur->rts_t );
            fprintf( stderr, " %6d", na_cur->other );

            fprintf( stderr, "\n" );

            na_cur = na_cur->next;
        }
    }
}