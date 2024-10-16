void dump_sort( void )
{
    time_t tt = time( NULL );

    /* thanks to Arnaud Cornet :-) */

    struct AP_info *new_ap_1st = NULL;
    struct AP_info *new_ap_end = NULL;

    struct ST_info *new_st_1st = NULL;
    struct ST_info *new_st_end = NULL;

    struct ST_info *st_cur, *st_min;
    struct AP_info *ap_cur, *ap_min;

    /* sort the aps by WHATEVER first */

    while( G.ap_1st )
    {
        ap_min = NULL;
        ap_cur = G.ap_1st;

        while( ap_cur != NULL )
        {
            if( tt - ap_cur->tlast > 20 )
                ap_min = ap_cur;

            ap_cur = ap_cur->next;
        }

        if( ap_min == NULL )
        {
            ap_min = ap_cur = G.ap_1st;

/*#define SORT_BY_BSSID	1
#define SORT_BY_POWER	2
#define SORT_BY_BEACON	3
#define SORT_BY_DATA	4
#define SORT_BY_PRATE	6
#define SORT_BY_CHAN	7
#define	SORT_BY_MBIT	8
#define SORT_BY_ENC	9
#define SORT_BY_CIPHER	10
#define SORT_BY_AUTH	11
#define SORT_BY_ESSID	12*/

	    while( ap_cur != NULL )
            {
		switch (G.sort_by) {
		    case SORT_BY_BSSID:
			if( memcmp(ap_cur->bssid,ap_min->bssid,6)*G.sort_inv < 0)
			    ap_min = ap_cur;
			break;
		    case SORT_BY_POWER:
			if( (ap_cur->avg_power - ap_min->avg_power)*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_BEACON:
			if( (ap_cur->nb_bcn < ap_min->nb_bcn)*G.sort_inv )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_DATA:
			if( (ap_cur->nb_data < ap_min->nb_data)*G.sort_inv )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_PRATE:
			if( (ap_cur->nb_dataps - ap_min->nb_dataps)*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_CHAN:
			if( (ap_cur->channel - ap_min->channel)*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_MBIT:
			if( (ap_cur->max_speed - ap_min->max_speed)*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_ENC:
			if( ((ap_cur->security&STD_FIELD) - (ap_min->security&STD_FIELD))*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_CIPHER:
			if( ((ap_cur->security&ENC_FIELD) - (ap_min->security&ENC_FIELD))*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_AUTH:
			if( ((ap_cur->security&AUTH_FIELD) - (ap_min->security&AUTH_FIELD))*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    case SORT_BY_ESSID:
			if( (strncasecmp((char*)ap_cur->essid, (char*)ap_min->essid, MAX_IE_ELEMENT_SIZE))*G.sort_inv < 0 )
			    ap_min = ap_cur;
			break;
		    default:	//sort by power
			if( ap_cur->avg_power < ap_min->avg_power)
			    ap_min = ap_cur;
			break;
		}
                ap_cur = ap_cur->next;
	    }
	}

        if( ap_min == G.ap_1st )
            G.ap_1st = ap_min->next;

        if( ap_min == G.ap_end )
            G.ap_end = ap_min->prev;

        if( ap_min->next )
            ap_min->next->prev = ap_min->prev;

        if( ap_min->prev )
            ap_min->prev->next = ap_min->next;

        if( new_ap_end )
        {
            new_ap_end->next = ap_min;
            ap_min->prev = new_ap_end;
            new_ap_end = ap_min;
            new_ap_end->next = NULL;
        }
        else
        {
            new_ap_1st = new_ap_end = ap_min;
            ap_min->next = ap_min->prev = NULL;
        }
    }

    G.ap_1st = new_ap_1st;
    G.ap_end = new_ap_end;

    /* now sort the stations */

    while( G.st_1st )
    {
        st_min = NULL;
        st_cur = G.st_1st;

        while( st_cur != NULL )
        {
            if( tt - st_cur->tlast > 60 )
                st_min = st_cur;

            st_cur = st_cur->next;
        }

        if( st_min == NULL )
        {
            st_min = st_cur = G.st_1st;

            while( st_cur != NULL )
            {
                if( st_cur->power < st_min->power)
                    st_min = st_cur;

                st_cur = st_cur->next;
            }
        }

        if( st_min == G.st_1st )
            G.st_1st = st_min->next;

        if( st_min == G.st_end )
            G.st_end = st_min->prev;

        if( st_min->next )
            st_min->next->prev = st_min->prev;

        if( st_min->prev )
            st_min->prev->next = st_min->next;

        if( new_st_end )
        {
            new_st_end->next = st_min;
            st_min->prev = new_st_end;
            new_st_end = st_min;
            new_st_end->next = NULL;
        }
        else
        {
            new_st_1st = new_st_end = st_min;
            st_min->next = st_min->prev = NULL;
        }
    }

    G.st_1st = new_st_1st;
    G.st_end = new_st_end;
}