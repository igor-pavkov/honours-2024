int dump_write_kismet_netxml( void )
{
    int network_number, average_power, client_nbr;
    int client_max_rate, unused;
    struct AP_info *ap_cur;
    struct ST_info *st_cur;
    char first_time[TIME_STR_LENGTH];
    char last_time[TIME_STR_LENGTH];
    char * manuf;
    char * essid = NULL;

    if (! G.record_data || !G.output_format_kismet_netxml)
    	return 0;

    fseek( G.f_kis_xml, 0, SEEK_SET );

	/* Header and airodump-ng start time */
    fprintf( G.f_kis_xml, "%s%s%s",
    		KISMET_NETXML_HEADER_BEGIN,
			G.airodump_start_time,
    		KISMET_NETXML_HEADER_END );


    ap_cur = G.ap_1st;

    network_number = 0;
    while( ap_cur != NULL )
    {
        if( memcmp( ap_cur->bssid, BROADCAST, 6 ) == 0 )
        {
            ap_cur = ap_cur->next;
            continue;
        }

        if(ap_cur->security != 0 && G.f_encrypt != 0 && ((ap_cur->security & G.f_encrypt) == 0))
        {
            ap_cur = ap_cur->next;
            continue;
        }

        if(is_filtered_essid(ap_cur->essid) || ap_cur->nb_pkt < 2 /* XXX: Maybe this last check should be removed */ )
        {
            ap_cur = ap_cur->next;
            continue;
        }

		++network_number; // Network Number
		strncpy(first_time, ctime(&ap_cur->tinit), TIME_STR_LENGTH - 1);
		first_time[strlen(first_time) - 1] = 0; // remove new line

		strncpy(last_time, ctime(&ap_cur->tlast), TIME_STR_LENGTH - 1);
		last_time[strlen(last_time) - 1] = 0; // remove new line

		fprintf(G.f_kis_xml, "\t<wireless-network number=\"%d\" type=\"infrastructure\" ",
			network_number);
		fprintf(G.f_kis_xml, "first-time=\"%s\" last-time=\"%s\">\n", first_time, last_time);

		fprintf(G.f_kis_xml, "\t\t<SSID first-time=\"%s\" last-time=\"%s\">\n",
				first_time, last_time);
		fprintf(G.f_kis_xml, "\t\t\t<type>Beacon</type>\n" );
		fprintf(G.f_kis_xml, "\t\t\t<max-rate>%d.000000</max-rate>\n", ap_cur->max_speed );
		fprintf(G.f_kis_xml, "\t\t\t<packets>%ld</packets>\n", ap_cur->nb_bcn );
		fprintf(G.f_kis_xml, "\t\t\t<beaconrate>%d</beaconrate>\n", 10 );
		fprintf(G.f_kis_xml, "\t\t\t<encryption>");
		//Encryption
		if( (ap_cur->security & (STD_OPN|STD_WEP|STD_WPA|STD_WPA2)) != 0)
		{
			if( ap_cur->security & STD_WPA2 ) fprintf( G.f_kis_xml, "WPA2 " );
			if( ap_cur->security & STD_WPA  ) fprintf( G.f_kis_xml, "WPA " );
			if( ap_cur->security & STD_WEP  ) fprintf( G.f_kis_xml, "WEP " );
			if( ap_cur->security & STD_OPN  ) fprintf( G.f_kis_xml, "OPN " );
		}

		if( (ap_cur->security & (ENC_WEP|ENC_TKIP|ENC_WRAP|ENC_CCMP|ENC_WEP104|ENC_WEP40)) != 0 )
		{
			if( ap_cur->security & ENC_CCMP   ) fprintf( G.f_kis_xml, "AES-CCM ");
			if( ap_cur->security & ENC_WRAP   ) fprintf( G.f_kis_xml, "WRAP ");
			if( ap_cur->security & ENC_TKIP   ) fprintf( G.f_kis_xml, "TKIP ");
			if( ap_cur->security & ENC_WEP104 ) fprintf( G.f_kis_xml, "WEP104 ");
			if( ap_cur->security & ENC_WEP40  ) fprintf( G.f_kis_xml, "WEP40 ");
/*      	if( ap_cur->security & ENC_WEP    ) fprintf( G.f_kis_xml, "WEP ");*/
		}
		fprintf(G.f_kis_xml, "</encryption>\n");

		/* ESSID */
		fprintf(G.f_kis_xml, "\t\t\t<essid cloaked=\"%s\">",
					(ap_cur->essid[0] == 0) ? "true" : "false");
		essid = sanitize_xml(ap_cur->essid, ap_cur->ssid_length);
		if (essid != NULL) {
			fprintf(G.f_kis_xml, "%s", essid);
			free(essid);
		}
		fprintf(G.f_kis_xml, "</essid>\n");

		/* End of SSID tag */
		fprintf(G.f_kis_xml, "\t\t</SSID>\n");

		/* BSSID */
		fprintf( G.f_kis_xml, "\t\t<BSSID>%02X:%02X:%02X:%02X:%02X:%02X</BSSID>\n",
					 ap_cur->bssid[0], ap_cur->bssid[1],
					 ap_cur->bssid[2], ap_cur->bssid[3],
					 ap_cur->bssid[4], ap_cur->bssid[5] );

		/* Manufacturer, if set using standard oui list */
		manuf = sanitize_xml((unsigned char *)ap_cur->manuf, strlen(ap_cur->manuf));
		fprintf(G.f_kis_xml, "\t\t<manuf>%s</manuf>\n", (manuf != NULL) ? manuf : "Unknown");
		free(manuf);

		/* Channel
		   FIXME: Take G.freqoption in account */
		fprintf(G.f_kis_xml, "\t\t<channel>%d</channel>\n", ap_cur->channel);

		/* Freq (in Mhz) and total number of packet on that frequency
		   FIXME: Take G.freqoption in account */
		fprintf(G.f_kis_xml, "\t\t<freqmhz>%d %ld</freqmhz>\n",
					getFrequencyFromChannel(ap_cur->channel),
					//ap_cur->nb_data + ap_cur->nb_bcn );
					ap_cur->nb_pkt );

		/* XXX: What about 5.5Mbit */
		fprintf(G.f_kis_xml, "\t\t<maxseenrate>%d</maxseenrate>\n", ap_cur->max_speed * 1000);

		/* Packets */
		fprintf(G.f_kis_xml, "\t\t<packets>\n"
					"\t\t\t<LLC>%ld</LLC>\n"
					"\t\t\t<data>%ld</data>\n"
					"\t\t\t<crypt>0</crypt>\n"
					"\t\t\t<total>%ld</total>\n"
					"\t\t\t<fragments>0</fragments>\n"
					"\t\t\t<retries>0</retries>\n"
					"\t\t</packets>\n",
					ap_cur->nb_bcn, ap_cur->nb_data,
					//ap_cur->nb_data + ap_cur->nb_bcn );
					ap_cur->nb_pkt );


		/*
		 * XXX: What does that field mean? Is it the total size of data?
		 *      It seems that 'd' is appended at the end for clients, why?
		 */
		fprintf(G.f_kis_xml, "\t\t<datasize>0</datasize>\n");

		/* Client information */
		st_cur = G.st_1st;
		client_nbr = 0;

		while ( st_cur != NULL )
		{
			/* If not associated or Broadcast Mac, try next one */
			if ( st_cur->base == NULL ||
				 memcmp( st_cur->stmac, BROADCAST, 6 ) == 0  )
			{
				st_cur = st_cur->next;
				continue;
			}

			/* Compare BSSID */
			if ( memcmp( st_cur->base->bssid, ap_cur->bssid, 6 ) != 0 )
			{
				st_cur = st_cur->next;
				continue;
			}

			++client_nbr;


			strncpy(first_time, ctime(&st_cur->tinit), TIME_STR_LENGTH - 1);
			first_time[strlen(first_time) - 1] = 0; // remove new line

			strncpy(last_time, ctime(&st_cur->tlast), TIME_STR_LENGTH - 1);
			last_time[strlen(last_time) - 1] = 0; // remove new line

			fprintf(G.f_kis_xml, "\t\t<wireless-client number=\"%d\" "
								 "type=\"established\" first-time=\"%s\""
								 " last-time=\"%s\">\n",
								 client_nbr, first_time, last_time );

			fprintf( G.f_kis_xml, "\t\t\t<client-mac>%02X:%02X:%02X:%02X:%02X:%02X</client-mac>\n",
						 st_cur->stmac[0], st_cur->stmac[1],
						 st_cur->stmac[2], st_cur->stmac[3],
						 st_cur->stmac[4], st_cur->stmac[5] );

			/* Manufacturer, if set using standard oui list */
			fprintf(G.f_kis_xml, "\t\t\t<client-manuf>%s</client-manuf>\n", (st_cur->manuf != NULL) ? st_cur->manuf : "Unknown");

			/* Channel
			   FIXME: Take G.freqoption in account */
			fprintf(G.f_kis_xml, "\t\t\t<channel>%d</channel>\n", ap_cur->channel);

			/* Rate: unaccurate because it's the latest rate seen */
			client_max_rate = ( st_cur->rate_from > st_cur->rate_to ) ? st_cur->rate_from : st_cur->rate_to ;
			fprintf(G.f_kis_xml, "\t\t\t<maxseenrate>%.6f</maxseenrate>\n", client_max_rate / 1000000.0 );

			/* Packets */
			fprintf(G.f_kis_xml, "\t\t\t<packets>\n"
						"\t\t\t\t<LLC>0</LLC>\n"
						"\t\t\t\t<data>0</data>\n"
						"\t\t\t\t<crypt>0</crypt>\n"
						"\t\t\t\t<total>%ld</total>\n"
						"\t\t\t\t<fragments>0</fragments>\n"
						"\t\t\t\t<retries>0</retries>\n"
						"\t\t\t</packets>\n",
						st_cur->nb_pkt );

			/* SNR information */
			average_power = (st_cur->power == -1) ? 0 : st_cur->power;
			fprintf(G.f_kis_xml, "\t\t\t<snr-info>\n"
					"\t\t\t\t<last_signal_dbm>%d</last_signal_dbm>\n"
					"\t\t\t\t<last_noise_dbm>0</last_noise_dbm>\n"
					"\t\t\t\t<last_signal_rssi>%d</last_signal_rssi>\n"
					"\t\t\t\t<last_noise_rssi>0</last_noise_rssi>\n"
					"\t\t\t\t<min_signal_dbm>%d</min_signal_dbm>\n"
					"\t\t\t\t<min_noise_dbm>0</min_noise_dbm>\n"
					"\t\t\t\t<min_signal_rssi>1024</min_signal_rssi>\n"
					"\t\t\t\t<min_noise_rssi>1024</min_noise_rssi>\n"
					"\t\t\t\t<max_signal_dbm>%d</max_signal_dbm>\n"
					"\t\t\t\t<max_noise_dbm>0</max_noise_dbm>\n"
					"\t\t\t\t<max_signal_rssi>%d</max_signal_rssi>\n"
					"\t\t\t\t<max_noise_rssi>0</max_noise_rssi>\n"
					 "\t\t\t</snr-info>\n",
					 average_power, average_power, average_power,
					 average_power, average_power );

			/* GPS Coordinates
			   XXX: We don't have GPS coordinates for clients */
			if (G.usegpsd)
			{
				fprintf(G.f_kis_xml, "\t\t<gps-info>\n"
							"\t\t\t<min-lat>%.6f</min-lat>\n"
							"\t\t\t<min-lon>%.6f</min-lon>\n"
							"\t\t\t<min-alt>%.6f</min-alt>\n"
							"\t\t\t<min-spd>%.6f</min-spd>\n"
							"\t\t\t<max-lat>%.6f</max-lat>\n"
							"\t\t\t<max-lon>%.6f</max-lon>\n"
							"\t\t\t<max-alt>%.6f</max-alt>\n"
							"\t\t\t<max-spd>%.6f</max-spd>\n"
							"\t\t\t<peak-lat>%.6f</peak-lat>\n"
							"\t\t\t<peak-lon>%.6f</peak-lon>\n"
							"\t\t\t<peak-alt>%.6f</peak-alt>\n"
							"\t\t\t<avg-lat>%.6f</avg-lat>\n"
							"\t\t\t<avg-lon>%.6f</avg-lon>\n"
							"\t\t\t<avg-alt>%.6f</avg-alt>\n"
							 "\t\t</gps-info>\n",
							 0.0, 0.0, 0.0, 0.0,
							 0.0, 0.0, 0.0, 0.0,
							 0.0, 0.0, 0.0,
							 0.0, 0.0, 0.0 );
			}


			/* Trailing information */
			fprintf(G.f_kis_xml, "\t\t\t<cdp-device></cdp-device>\n"
								 "\t\t\t<cdp-portid></cdp-portid>\n");
			fprintf(G.f_kis_xml, "\t\t</wireless-client>\n" );

			/* Next client */
			st_cur = st_cur->next;
		}

		/* SNR information */
		average_power = (ap_cur->avg_power == -1) ? 0 : ap_cur->avg_power;
		fprintf(G.f_kis_xml, "\t\t<snr-info>\n"
					"\t\t\t<last_signal_dbm>%d</last_signal_dbm>\n"
					"\t\t\t<last_noise_dbm>0</last_noise_dbm>\n"
					"\t\t\t<last_signal_rssi>%d</last_signal_rssi>\n"
					"\t\t\t<last_noise_rssi>0</last_noise_rssi>\n"
					"\t\t\t<min_signal_dbm>%d</min_signal_dbm>\n"
					"\t\t\t<min_noise_dbm>0</min_noise_dbm>\n"
					"\t\t\t<min_signal_rssi>1024</min_signal_rssi>\n"
					"\t\t\t<min_noise_rssi>1024</min_noise_rssi>\n"
					"\t\t\t<max_signal_dbm>%d</max_signal_dbm>\n"
					"\t\t\t<max_noise_dbm>0</max_noise_dbm>\n"
					"\t\t\t<max_signal_rssi>%d</max_signal_rssi>\n"
					"\t\t\t<max_noise_rssi>0</max_noise_rssi>\n"
					 "\t\t</snr-info>\n",
					 average_power, average_power, average_power,
					 average_power, average_power );

		/* GPS Coordinates */
		if (G.usegpsd)
		{
			fprintf(G.f_kis_xml, "\t\t<gps-info>\n"
						"\t\t\t<min-lat>%.6f</min-lat>\n"
						"\t\t\t<min-lon>%.6f</min-lon>\n"
						"\t\t\t<min-alt>%.6f</min-alt>\n"
						"\t\t\t<min-spd>%.6f</min-spd>\n"
						"\t\t\t<max-lat>%.6f</max-lat>\n"
						"\t\t\t<max-lon>%.6f</max-lon>\n"
						"\t\t\t<max-alt>%.6f</max-alt>\n"
						"\t\t\t<max-spd>%.6f</max-spd>\n"
						"\t\t\t<peak-lat>%.6f</peak-lat>\n"
						"\t\t\t<peak-lon>%.6f</peak-lon>\n"
						"\t\t\t<peak-alt>%.6f</peak-alt>\n"
						"\t\t\t<avg-lat>%.6f</avg-lat>\n"
						"\t\t\t<avg-lon>%.6f</avg-lon>\n"
						"\t\t\t<avg-alt>%.6f</avg-alt>\n"
						 "\t\t</gps-info>\n",
						ap_cur->gps_loc_min[0],
						ap_cur->gps_loc_min[1],
						ap_cur->gps_loc_min[2],
						ap_cur->gps_loc_min[3],
						ap_cur->gps_loc_max[0],
						ap_cur->gps_loc_max[1],
						ap_cur->gps_loc_max[2],
						ap_cur->gps_loc_max[3],
						ap_cur->gps_loc_best[0],
						ap_cur->gps_loc_best[1],
						ap_cur->gps_loc_best[2],
						/* Can the "best" be considered as average??? */
						ap_cur->gps_loc_best[0],
						ap_cur->gps_loc_best[1],
						ap_cur->gps_loc_best[2] );
		}

		/* Trailing information */
		fprintf(G.f_kis_xml, "\t\t<bsstimestamp>0</bsstimestamp>\n"
					 "\t\t<cdp-device></cdp-device>\n"
					 "\t\t<cdp-portid></cdp-portid>\n");

		/* Closing tag for the current wireless network */
		fprintf(G.f_kis_xml, "\t</wireless-network>\n");
		//-------- End of XML

        ap_cur = ap_cur->next;
    }

	/* Trailing */
    fprintf( G.f_kis_xml, "%s\n", KISMET_NETXML_TRAILER );

    fflush( G.f_kis_xml );

    /* Sometimes there can be crap at the end of the file, so truncating is a good idea.
       XXX: Is this really correct, I hope fileno() won't have any side effect */
	unused = ftruncate(fileno(G.f_kis_xml), ftell( G.f_kis_xml ) );

    return 0;
}