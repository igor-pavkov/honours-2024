int check_monitor(struct wif *wi[], int *fd_raw, int *fdh, int cards)
{
    int i, monitor;
    char ifname[64];

    for(i=0; i<cards; i++)
    {
        monitor = wi_get_monitor(wi[i]);
        if(monitor != 0)
        {
            memset(G.message, '\x00', sizeof(G.message));
            snprintf(G.message, sizeof(G.message), "][ %s reset to monitor mode", wi_get_ifname(wi[i]));
            //reopen in monitor mode

            strncpy(ifname, wi_get_ifname(wi[i]), sizeof(ifname)-1);
            ifname[sizeof(ifname)-1] = 0;

            wi_close(wi[i]);
            wi[i] = wi_open(ifname);
            if (!wi[i]) {
                printf("Can't reopen %s\n", ifname);
                exit(1);
            }

            fd_raw[i] = wi_fd(wi[i]);
            if (fd_raw[i] > *fdh)
                *fdh = fd_raw[i];
        }
    }
    return 0;
}