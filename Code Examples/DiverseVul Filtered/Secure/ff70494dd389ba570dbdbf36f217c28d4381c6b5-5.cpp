int dump_write_csv( void )
{
    int i, j, n;
    struct tm *ltime;
    char ssid_list[512];
    struct AP_info *ap_cur;
    struct ST_info *st_cur;

    if (! G.record_data || !G.output_format_csv)
    	return 0;

    fseek( G.f_txt, 0, SEEK_SET );

    fprintf( G.f_txt,
        "\r\nBSSID, First time seen, Last time seen, channel, Speed, "
        "Privacy, Cipher, Authentication, Power, # beacons, # IV, LAN IP, ID-length, ESSID, Key\r\n" );

    ap_cur = G.ap_1st;

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

        if(is_filtered_essid(ap_cur->essid) || ap_cur->nb_pkt < 2)
        {
            ap_cur = ap_cur->next;
            continue;
        }

        fprintf( G.f_txt, "%02X:%02X:%02X:%02X:%02X:%02X, ",
                 ap_cur->bssid[0], ap_cur->bssid[1],
                 ap_cur->bssid[2], ap_cur->bssid[3],
                 ap_cur->bssid[4], ap_cur->bssid[5] );

        ltime = localtime( &ap_cur->tinit );

        fprintf( G.f_txt, "%04d-%02d-%02d %02d:%02d:%02d, ",
                 1900 + ltime->tm_year, 1 + ltime->tm_mon,
                 ltime->tm_mday, ltime->tm_hour,
                 ltime->tm_min,  ltime->tm_sec );

        ltime = localtime( &ap_cur->tlast );

        fprintf( G.f_txt, "%04d-%02d-%02d %02d:%02d:%02d, ",
                 1900 + ltime->tm_year, 1 + ltime->tm_mon,
                 ltime->tm_mday, ltime->tm_hour,
                 ltime->tm_min,  ltime->tm_sec );

        fprintf( G.f_txt, "%2d, %3d, ",
                 ap_cur->channel,
                 ap_cur->max_speed );

        if( (ap_cur->security & (STD_OPN|STD_WEP|STD_WPA|STD_WPA2)) == 0) fprintf( G.f_txt, "    " );
        else
        {
            if( ap_cur->security & STD_WPA2 ) fprintf( G.f_txt, "WPA2" );
            if( ap_cur->security & STD_WPA  ) fprintf( G.f_txt, "WPA " );
            if( ap_cur->security & STD_WEP  ) fprintf( G.f_txt, "WEP " );
            if( ap_cur->security & STD_OPN  ) fprintf( G.f_txt, "OPN " );
        }

        fprintf( G.f_txt, ",");

        if( (ap_cur->security & (ENC_WEP|ENC_TKIP|ENC_WRAP|ENC_CCMP|ENC_WEP104|ENC_WEP40)) == 0 ) fprintf( G.f_txt, "       ");
        else
        {
            if( ap_cur->security & ENC_CCMP   ) fprintf( G.f_txt, " CCMP");
            if( ap_cur->security & ENC_WRAP   ) fprintf( G.f_txt, " WRAP");
            if( ap_cur->security & ENC_TKIP   ) fprintf( G.f_txt, " TKIP");
            if( ap_cur->security & ENC_WEP104 ) fprintf( G.f_txt, " WEP104");
            if( ap_cur->security & ENC_WEP40  ) fprintf( G.f_txt, " WEP40");
            if( ap_cur->security & ENC_WEP    ) fprintf( G.f_txt, " WEP");
        }

        fprintf( G.f_txt, ",");

        if( (ap_cur->security & (AUTH_OPN|AUTH_PSK|AUTH_MGT)) == 0 ) fprintf( G.f_txt, "   ");
        else
        {
            if( ap_cur->security & AUTH_MGT   ) fprintf( G.f_txt, " MGT");
            if( ap_cur->security & AUTH_PSK   )
			{
				if( ap_cur->security & STD_WEP )
					fprintf( G.f_txt, "SKA");
				else
					fprintf( G.f_txt, "PSK");
			}
            if( ap_cur->security & AUTH_OPN   ) fprintf( G.f_txt, " OPN");
        }

        fprintf( G.f_txt, ", %3d, %8ld, %8ld, ",
                 ap_cur->avg_power,
                 ap_cur->nb_bcn,
                 ap_cur->nb_data );

        fprintf( G.f_txt, "%3d.%3d.%3d.%3d, ",
                 ap_cur->lanip[0], ap_cur->lanip[1],
                 ap_cur->lanip[2], ap_cur->lanip[3] );

        fprintf( G.f_txt, "%3d, ", ap_cur->ssid_length);

        for(i=0; i<ap_cur->ssid_length; i++)
        {
            fprintf( G.f_txt, "%c", ap_cur->essid[i] );
        }
        fprintf( G.f_txt, ", " );


        if(ap_cur->key != NULL)
        {
            for(i=0; i<(int)strlen(ap_cur->key); i++)
            {
                fprintf( G.f_txt, "%02X", ap_cur->key[i]);
                if(i<(int)(strlen(ap_cur->key)-1))
                    fprintf( G.f_txt, ":");
            }
        }

        fprintf( G.f_txt, "\r\n");

        ap_cur = ap_cur->next;
    }

    fprintf( G.f_txt,
        "\r\nStation MAC, First time seen, Last time seen, "
        "Power, # packets, BSSID, Probed ESSIDs\r\n" );

    st_cur = G.st_1st;

    while( st_cur != NULL )
    {
        ap_cur = st_cur->base;

        if( ap_cur->nb_pkt < 2 )
        {
            st_cur = st_cur->next;
            continue;
        }

        fprintf( G.f_txt, "%02X:%02X:%02X:%02X:%02X:%02X, ",
                 st_cur->stmac[0], st_cur->stmac[1],
                 st_cur->stmac[2], st_cur->stmac[3],
                 st_cur->stmac[4], st_cur->stmac[5] );

        ltime = localtime( &st_cur->tinit );

        fprintf( G.f_txt, "%04d-%02d-%02d %02d:%02d:%02d, ",
                 1900 + ltime->tm_year, 1 + ltime->tm_mon,
                 ltime->tm_mday, ltime->tm_hour,
                 ltime->tm_min,  ltime->tm_sec );

        ltime = localtime( &st_cur->tlast );

        fprintf( G.f_txt, "%04d-%02d-%02d %02d:%02d:%02d, ",
                 1900 + ltime->tm_year, 1 + ltime->tm_mon,
                 ltime->tm_mday, ltime->tm_hour,
                 ltime->tm_min,  ltime->tm_sec );

        fprintf( G.f_txt, "%3d, %8ld, ",
                 st_cur->power,
                 st_cur->nb_pkt );

        if( ! memcmp( ap_cur->bssid, BROADCAST, 6 ) )
            fprintf( G.f_txt, "(not associated) ," );
        else
            fprintf( G.f_txt, "%02X:%02X:%02X:%02X:%02X:%02X,",
                     ap_cur->bssid[0], ap_cur->bssid[1],
                     ap_cur->bssid[2], ap_cur->bssid[3],
                     ap_cur->bssid[4], ap_cur->bssid[5] );

        memset( ssid_list, 0, sizeof( ssid_list ) );

        for( i = 0, n = 0; i < NB_PRB; i++ )
        {
            if( st_cur->probes[i][0] == '\0' )
                continue;

            snprintf( ssid_list + n, sizeof( ssid_list ) - n - 1,
                      "%c", ( i > 0 ) ? ',' : ' ' );

            for(j=0; j<st_cur->ssid_length[i]; j++)
            {
                snprintf( ssid_list + n + 1 + j, sizeof( ssid_list ) - n - 2 - j,
                          "%c", st_cur->probes[i][j]);
            }

            n += ( 1 + st_cur->ssid_length[i] );
            if( n >= (int) sizeof( ssid_list ) )
                break;
        }

        fprintf( G.f_txt, "%s\r\n", ssid_list );

        st_cur = st_cur->next;
    }

    fprintf( G.f_txt, "\r\n" );
    fflush( G.f_txt );
    return 0;
}