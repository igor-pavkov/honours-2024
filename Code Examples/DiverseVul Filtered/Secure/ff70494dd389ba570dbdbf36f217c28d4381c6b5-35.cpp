int is_filtered_netmask(unsigned char *bssid)
{
    unsigned char mac1[6];
    unsigned char mac2[6];
    int i;

    for(i=0; i<6; i++)
    {
        mac1[i] = bssid[i]     & G.f_netmask[i];
        mac2[i] = G.f_bssid[i] & G.f_netmask[i];
    }

    if( memcmp(mac1, mac2, 6) != 0 )
    {
        return( 1 );
    }

    return 0;
}