int set_final_arp(unsigned char *buf, unsigned char *mymac)
{
    if(buf == NULL)
        return -1;

    //shifted by 10bytes to set source IP as target IP :)

    buf[0] = 0x08; // IP
    buf[1] = 0x00;
    buf[2] = 0x06; //hardware size
    buf[3] = 0x04; //protocol size
    buf[4] = 0x00;
    buf[5] = 0x01; //request
    memcpy(buf+6, mymac, 6); //sender mac
    buf[12] = 0xA9; //sender IP 169.254.87.197
    buf[13] = 0xFE;
    buf[14] = 0x57;
    buf[15] = 0xC5; //end sender IP

    return 0;
}