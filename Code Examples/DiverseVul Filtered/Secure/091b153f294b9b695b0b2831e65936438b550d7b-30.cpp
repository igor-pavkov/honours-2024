int set_clear_arp(unsigned char *buf, unsigned char *smac, unsigned char *dmac) //set first 22 bytes
{
    if(buf == NULL)
        return -1;

    memcpy(buf, S_LLC_SNAP_ARP, 8);
    buf[8]  = 0x00;
    buf[9]  = 0x01; //ethernet
    buf[10] = 0x08; // IP
    buf[11] = 0x00;
    buf[12] = 0x06; //hardware size
    buf[13] = 0x04; //protocol size
    buf[14] = 0x00;
    if(memcmp(dmac, BROADCAST, 6) == 0)
        buf[15]  = 0x01; //request
    else
        buf[15]  = 0x02; //reply
    memcpy(buf+16, smac, 6);

    return 0;
}