int filter_packet( unsigned char *h80211, int caplen )
{
    int z, mi_b, mi_s, mi_d, ext=0, qos;

    if(caplen <= 0)
        return( 1 );

    z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
    if ( ( h80211[0] & 0x80 ) == 0x80 )
    {
        qos = 1; /* 802.11e QoS */
        z+=2;
    }

    if( (h80211[0] & 0x0C) == 0x08)    //if data packet
        ext = z-24; //how many bytes longer than default ieee80211 header

    /* check length */
    if( caplen-ext < opt.f_minlen ||
        caplen-ext > opt.f_maxlen ) return( 1 );

    /* check the frame control bytes */

    if( ( h80211[0] & 0x0C ) != ( opt.f_type    << 2 ) &&
        opt.f_type    >= 0 ) return( 1 );

    if( ( h80211[0] & 0x70 ) != (( opt.f_subtype << 4 ) & 0x70) && //ignore the leading bit (QoS)
        opt.f_subtype >= 0 ) return( 1 );

    if( ( h80211[1] & 0x01 ) != ( opt.f_tods         ) &&
        opt.f_tods    >= 0 ) return( 1 );

    if( ( h80211[1] & 0x02 ) != ( opt.f_fromds  << 1 ) &&
        opt.f_fromds  >= 0 ) return( 1 );

    if( ( h80211[1] & 0x40 ) != ( opt.f_iswep   << 6 ) &&
        opt.f_iswep   >= 0 ) return( 1 );

    /* check the extended IV (TKIP) flag */

    if( opt.f_type == 2 && opt.f_iswep == 1 &&
        ( h80211[z + 3] & 0x20 ) != 0 ) return( 1 );

    /* MAC address checking */

    switch( h80211[1] & 3 )
    {
        case  0: mi_b = 16; mi_s = 10; mi_d =  4; break;
        case  1: mi_b =  4; mi_s = 10; mi_d = 16; break;
        case  2: mi_b = 10; mi_s = 16; mi_d =  4; break;
        default: mi_b = 10; mi_d = 16; mi_s = 24; break;
    }

    if( memcmp( opt.f_bssid, NULL_MAC, 6 ) != 0 )
        if( memcmp( h80211 + mi_b, opt.f_bssid, 6 ) != 0 )
            return( 1 );

    if( memcmp( opt.f_smac,  NULL_MAC, 6 ) != 0 )
        if( memcmp( h80211 + mi_s,  opt.f_smac,  6 ) != 0 )
            return( 1 );

    if( memcmp( opt.f_dmac,  NULL_MAC, 6 ) != 0 )
        if( memcmp( h80211 + mi_d,  opt.f_dmac,  6 ) != 0 )
            return( 1 );

    /* this one looks good */

    return( 0 );
}