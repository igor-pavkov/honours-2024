int set_bitrate(struct wif *wi, int rate)
{
    int i, newrate;

    if( wi_set_rate(wi, rate) )
        return 1;

//    if( reset_ifaces() )
//        return 1;

    //Workaround for buggy drivers (rt73) that do not accept 5.5M, but 5M instead
    if (rate == 5500000 && wi_get_rate(wi) != 5500000) {
	if( wi_set_rate(wi, 5000000) )
	    return 1;
    }

    newrate = wi_get_rate(wi);
    for(i=0; i<RATE_NUM; i++)
    {
        if(bitrates[i] == rate)
            break;
    }
    if(i==RATE_NUM)
        i=-1;
    if( newrate != rate )
    {
        if(i!=-1)
        {
            if( i>0 )
            {
                if(bitrates[i-1] >= newrate)
                {
                    printf("Couldn't set rate to %.1fMBit. (%.1fMBit instead)\n", (rate/1000000.0), (wi_get_rate(wi)/1000000.0));
                    return 1;
                }
            }
            if( i<RATE_NUM-1 )
            {
                if(bitrates[i+1] <= newrate)
                {
                    printf("Couldn't set rate to %.1fMBit. (%.1fMBit instead)\n", (rate/1000000.0), (wi_get_rate(wi)/1000000.0));
                    return 1;
                }
            }
            return 0;
        }
        printf("Couldn't set rate to %.1fMBit. (%.1fMBit instead)\n", (rate/1000000.0), (wi_get_rate(wi)/1000000.0));
        return 1;
    }
    return 0;
}