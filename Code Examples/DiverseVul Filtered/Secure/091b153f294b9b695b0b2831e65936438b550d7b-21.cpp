int set_clear_ip(unsigned char *buf, int ip_len) //set first 9 bytes
{
    if(buf == NULL)
        return -1;

    memcpy(buf, S_LLC_SNAP_IP, 8);
    buf[8]  = 0x45;
    buf[10] = (ip_len >> 8)  & 0xFF;
    buf[11] = ip_len & 0xFF;

    return 0;
}