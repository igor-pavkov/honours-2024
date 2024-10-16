void usb_port_location(USBPort *downstream, USBPort *upstream, int portnr)
{
    if (upstream) {
        snprintf(downstream->path, sizeof(downstream->path), "%s.%d",
                 upstream->path, portnr);
        downstream->hubcount = upstream->hubcount + 1;
    } else {
        snprintf(downstream->path, sizeof(downstream->path), "%d", portnr);
        downstream->hubcount = 0;
    }
}