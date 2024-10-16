int attack_check(unsigned char* bssid, char* essid, unsigned char* capa, struct wif *wi)
{
    int ap_chan=0, iface_chan=0;

    iface_chan = wi_get_channel(wi);

    if(iface_chan == -1 && !opt.ignore_negative_one)
    {
        PCT; printf("Couldn't determine current channel for %s, you should either force the operation with --ignore-negative-one or apply a kernel patch\n",
                wi_get_ifname(wi));
        return -1;
    }

    if(bssid != NULL)
    {
        ap_chan = wait_for_beacon(bssid, capa, essid);
        if(ap_chan < 0)
        {
            PCT; printf("No such BSSID available.\n");
            return -1;
        }
        if((ap_chan != iface_chan) && (iface_chan != -1 || !opt.ignore_negative_one))
        {
            PCT; printf("%s is on channel %d, but the AP uses channel %d\n", wi_get_ifname(wi), iface_chan, ap_chan);
            return -1;
        }
    }

    return 0;
}