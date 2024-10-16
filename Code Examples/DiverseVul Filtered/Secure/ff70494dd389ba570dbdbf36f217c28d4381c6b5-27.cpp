int get_ap_list_count() {
    time_t tt;
    struct tm *lt;
    struct AP_info *ap_cur;

    int num_ap;

    tt = time( NULL );
    lt = localtime( &tt );

    ap_cur = G.ap_end;

    num_ap = 0;

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
	ap_cur = ap_cur->prev;
    }

    return num_ap;
}