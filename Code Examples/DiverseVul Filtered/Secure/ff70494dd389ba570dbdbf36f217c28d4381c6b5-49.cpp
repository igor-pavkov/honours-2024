int dump_write_kismet_csv( void )
{
    int i, k;
//     struct tm *ltime;
/*    char ssid_list[512];*/
    struct AP_info *ap_cur;

    if (! G.record_data || !G.output_format_kismet_csv)
    	return 0;

    fseek( G.f_kis, 0, SEEK_SET );

    fprintf( G.f_kis, KISMET_HEADER );

    ap_cur = G.ap_1st;

    k=1;
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

        //Network
        fprintf( G.f_kis, "%d;", k );

        //NetType
        fprintf( G.f_kis, "infrastructure;");

        //ESSID
        for(i=0; i<ap_cur->ssid_length; i++)
        {
            fprintf( G.f_kis, "%c", ap_cur->essid[i] );
        }
        fprintf( G.f_kis, ";" );

        //BSSID
        fprintf( G.f_kis, "%02X:%02X:%02X:%02X:%02X:%02X;",
                 ap_cur->bssid[0], ap_cur->bssid[1],
                 ap_cur->bssid[2], ap_cur->bssid[3],
                 ap_cur->bssid[4], ap_cur->bssid[5] );

        //Info
        fprintf( G.f_kis, ";");

        //Channel
        fprintf( G.f_kis, "%d;", ap_cur->channel);

        //Cloaked
        fprintf( G.f_kis, "No;");

        //Encryption
        if( (ap_cur->security & (STD_OPN|STD_WEP|STD_WPA|STD_WPA2)) != 0)
        {
            if( ap_cur->security & STD_WPA2 ) fprintf( G.f_kis, "WPA2," );
            if( ap_cur->security & STD_WPA  ) fprintf( G.f_kis, "WPA," );
            if( ap_cur->security & STD_WEP  ) fprintf( G.f_kis, "WEP," );
            if( ap_cur->security & STD_OPN  ) fprintf( G.f_kis, "OPN," );
        }

        if( (ap_cur->security & (ENC_WEP|ENC_TKIP|ENC_WRAP|ENC_CCMP|ENC_WEP104|ENC_WEP40)) == 0 ) fprintf( G.f_kis, "None,");
        else
        {
            if( ap_cur->security & ENC_CCMP   ) fprintf( G.f_kis, "AES-CCM,");
            if( ap_cur->security & ENC_WRAP   ) fprintf( G.f_kis, "WRAP,");
            if( ap_cur->security & ENC_TKIP   ) fprintf( G.f_kis, "TKIP,");
            if( ap_cur->security & ENC_WEP104 ) fprintf( G.f_kis, "WEP104,");
            if( ap_cur->security & ENC_WEP40  ) fprintf( G.f_kis, "WEP40,");
/*            if( ap_cur->security & ENC_WEP    ) fprintf( G.f_kis, " WEP,");*/
        }

        fseek(G.f_kis, -1, SEEK_CUR);
        fprintf(G.f_kis, ";");

        //Decrypted
        fprintf( G.f_kis, "No;");

        //MaxRate
        fprintf( G.f_kis, "%d.0;", ap_cur->max_speed );

        //MaxSeenRate
        fprintf( G.f_kis, "0;");

        //Beacon
        fprintf( G.f_kis, "%ld;", ap_cur->nb_bcn);

        //LLC
        fprintf( G.f_kis, "0;");

        //Data
        fprintf( G.f_kis, "%ld;", ap_cur->nb_data );

        //Crypt
        fprintf( G.f_kis, "0;");

        //Weak
        fprintf( G.f_kis, "0;");

        //Total
        fprintf( G.f_kis, "%ld;", ap_cur->nb_data );

        //Carrier
        fprintf( G.f_kis, ";");

        //Encoding
        fprintf( G.f_kis, ";");

        //FirstTime
        fprintf( G.f_kis, "%s", ctime(&ap_cur->tinit) );
        fseek(G.f_kis, -1, SEEK_CUR);
        fprintf( G.f_kis, ";");

        //LastTime
        fprintf( G.f_kis, "%s", ctime(&ap_cur->tlast) );
        fseek(G.f_kis, -1, SEEK_CUR);
        fprintf( G.f_kis, ";");

        //BestQuality
        fprintf( G.f_kis, "%d;", ap_cur->avg_power );

        //BestSignal
        fprintf( G.f_kis, "0;" );

        //BestNoise
        fprintf( G.f_kis, "0;" );

        //GPSMinLat
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_min[0]);

        //GPSMinLon
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_min[1]);

        //GPSMinAlt
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_min[2]);

        //GPSMinSpd
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_min[3]);

        //GPSMaxLat
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_max[0]);

        //GPSMaxLon
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_max[1]);

        //GPSMaxAlt
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_max[2]);

        //GPSMaxSpd
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_max[3]);

        //GPSBestLat
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_best[0]);

        //GPSBestLon
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_best[1]);

        //GPSBestAlt
        fprintf( G.f_kis, "%.6f;", ap_cur->gps_loc_best[2]);

        //DataSize
        fprintf( G.f_kis, "0;" );

        //IPType
        fprintf( G.f_kis, "0;" );

        //IP
        fprintf( G.f_kis, "%d.%d.%d.%d;",
                 ap_cur->lanip[0], ap_cur->lanip[1],
                 ap_cur->lanip[2], ap_cur->lanip[3] );

        fprintf( G.f_kis, "\r\n");

        ap_cur = ap_cur->next;
        k++;
    }

    fflush( G.f_kis );
    return 0;
}