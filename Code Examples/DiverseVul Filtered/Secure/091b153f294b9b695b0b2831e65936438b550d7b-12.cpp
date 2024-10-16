int set_final_ip(unsigned char *buf, unsigned char *mymac)
{
    if(buf == NULL)
        return -1;

    //shifted by 10bytes to set source IP as target IP :)

    buf[0] = 0x06; //hardware size
    buf[1] = 0x04; //protocol size
    buf[2] = 0x00;
    buf[3] = 0x01; //request
    memcpy(buf+4, mymac, 6); //sender mac
    buf[10] = 0xA9; //sender IP from 169.254.XXX.XXX
    buf[11] = 0xFE;

    return 0;
}