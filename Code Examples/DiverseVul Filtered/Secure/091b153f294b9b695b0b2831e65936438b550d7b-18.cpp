int getnet( unsigned char* capa, int filter, int force)
{
    unsigned char *bssid;

    if(opt.nodetect)
        return 0;

    if(filter)
        bssid = opt.f_bssid;
    else
        bssid = opt.r_bssid;


    if( memcmp(bssid, NULL_MAC, 6) )
    {
        PCT; printf("Waiting for beacon frame (BSSID: %02X:%02X:%02X:%02X:%02X:%02X) on channel %d\n",
                    bssid[0],bssid[1],bssid[2],bssid[3],bssid[4],bssid[5],wi_get_channel(_wi_in));
    }
    else if(strlen(opt.r_essid) > 0)
    {
        PCT; printf("Waiting for beacon frame (ESSID: %s) on channel %d\n", opt.r_essid,wi_get_channel(_wi_in));
    }
    else if(force)
    {
        PCT;
        if(filter)
        {
            printf("Please specify at least a BSSID (-b) or an ESSID (-e)\n");
        }
        else
        {
            printf("Please specify at least a BSSID (-a) or an ESSID (-e)\n");
        }
        return( 1 );
    }
    else
        return 0;

    if( attack_check(bssid, opt.r_essid, capa, _wi_in) != 0)
    {
        if(memcmp(bssid, NULL_MAC, 6))
        {
            if( strlen(opt.r_essid) == 0 || opt.r_essid[0] < 32)
            {
                printf( "Please specify an ESSID (-e).\n" );
            }
        }

        if(!memcmp(bssid, NULL_MAC, 6))
        {
            if(strlen(opt.r_essid) > 0)
            {
                printf( "Please specify a BSSID (-a).\n" );
            }
        }
        return( 1 );
    }

    return 0;
}