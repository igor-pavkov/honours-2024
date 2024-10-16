int get_sta_list_count() {
    time_t tt;
    struct tm *lt;
    struct AP_info *ap_cur;
    struct ST_info *st_cur;

    int num_sta;

    tt = time( NULL );
    lt = localtime( &tt );

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

        st_cur = G.st_end;

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

            st_cur = st_cur->prev;
        }

        ap_cur = ap_cur->prev;
    }
    return num_sta;
}