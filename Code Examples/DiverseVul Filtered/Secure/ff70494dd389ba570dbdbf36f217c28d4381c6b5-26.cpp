void update_rx_quality( )
{
    unsigned int time_diff, capt_time, miss_time;
    int missed_frames;
    struct AP_info *ap_cur = NULL;
    struct ST_info *st_cur = NULL;
    struct timeval cur_time;

    ap_cur = G.ap_1st;
    st_cur = G.st_1st;

    gettimeofday( &cur_time, NULL );

    /* accesspoints */
    while( ap_cur != NULL )
    {
        time_diff = 1000000 * (cur_time.tv_sec  - ap_cur->ftimer.tv_sec )
                            + (cur_time.tv_usec - ap_cur->ftimer.tv_usec);

        /* update every `QLT_TIME`seconds if the rate is low, or every 500ms otherwise */
        if( (ap_cur->fcapt >= QLT_COUNT && time_diff > 500000 ) || time_diff > (QLT_TIME * 1000000) )
        {
            /* at least one frame captured */
            if(ap_cur->fcapt > 1)
            {
                capt_time =   ( 1000000 * (ap_cur->ftimel.tv_sec  - ap_cur->ftimef.tv_sec )    //time between first and last captured frame
                                        + (ap_cur->ftimel.tv_usec - ap_cur->ftimef.tv_usec) );

                miss_time =   ( 1000000 * (ap_cur->ftimef.tv_sec  - ap_cur->ftimer.tv_sec )    //time between timer reset and first frame
                                        + (ap_cur->ftimef.tv_usec - ap_cur->ftimer.tv_usec) )
                            + ( 1000000 * (cur_time.tv_sec  - ap_cur->ftimel.tv_sec )          //time between last frame and this moment
                                        + (cur_time.tv_usec - ap_cur->ftimel.tv_usec) );

                //number of frames missed at the time where no frames were captured; extrapolated by assuming a constant framerate
                if(capt_time > 0 && miss_time > 200000)
                {
                    missed_frames = ((float)((float)miss_time/(float)capt_time) * ((float)ap_cur->fcapt + (float)ap_cur->fmiss));
                    ap_cur->fmiss += missed_frames;
                }

                ap_cur->rx_quality = ((float)((float)ap_cur->fcapt / ((float)ap_cur->fcapt + (float)ap_cur->fmiss)) * 100.0);
            }
            else ap_cur->rx_quality = 0; /* no packets -> zero quality */

            /* normalize, in case the seq numbers are not iterating */
            if(ap_cur->rx_quality > 100) ap_cur->rx_quality = 100;
            if(ap_cur->rx_quality < 0  ) ap_cur->rx_quality =   0;

            /* reset variables */
            ap_cur->fcapt = 0;
            ap_cur->fmiss = 0;
            gettimeofday( &(ap_cur->ftimer) ,NULL);
        }
        ap_cur = ap_cur->next;
    }

    /* stations */
    while( st_cur != NULL )
    {
        time_diff = 1000000 * (cur_time.tv_sec  - st_cur->ftimer.tv_sec )
                            + (cur_time.tv_usec - st_cur->ftimer.tv_usec);

        if( time_diff > 10000000 )
        {
            st_cur->missed = 0;
            gettimeofday( &(st_cur->ftimer), NULL );
        }

        st_cur = st_cur->next;
    }

}