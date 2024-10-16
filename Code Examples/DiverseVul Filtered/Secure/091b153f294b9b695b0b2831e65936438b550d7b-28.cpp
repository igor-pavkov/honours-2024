int grab_essid(unsigned char* packet, int len)
{
    int i=0, j=0, pos=0, tagtype=0, taglen=0, chan=0;
    unsigned char bssid[6];

    memcpy(bssid, packet+16, 6);
    taglen = 22;    //initial value to get the fixed tags parsing started
    taglen+= 12;    //skip fixed tags in frames
    do
    {
        pos    += taglen + 2;
        tagtype = packet[pos];
        taglen  = packet[pos+1];
    } while(tagtype != 3 && pos < len-2);

    if(tagtype != 3) return -1;
    if(taglen != 1) return -1;
    if(pos+2+taglen > len) return -1;

    chan = packet[pos+2];

    pos=0;

    taglen = 22;    //initial value to get the fixed tags parsing started
    taglen+= 12;    //skip fixed tags in frames
    do
    {
        pos    += taglen + 2;
        tagtype = packet[pos];
        taglen  = packet[pos+1];
    } while(tagtype != 0 && pos < len-2);

    if(tagtype != 0) return -1;
    if(taglen > 250) taglen = 250;
    if(pos+2+taglen > len) return -1;

    for(i=0; i<20; i++)
    {
        if( ap[i].set)
        {
            if( memcmp(bssid, ap[i].bssid, 6) == 0 )    //got it already
            {
                if(packet[0] == 0x50 && !ap[i].found)
                {
                    ap[i].found++;
                }
                if(ap[i].chan == 0) ap[i].chan=chan;
                break;
            }
        }
        if(ap[i].set == 0)
        {
            for(j=0; j<taglen; j++)
            {
                if(packet[pos+2+j] < 32 || packet[pos+2+j] > 127)
                {
                    return -1;
                }
            }

            ap[i].set = 1;
            ap[i].len = taglen;
            memcpy(ap[i].essid, packet+pos+2, taglen);
            ap[i].essid[taglen] = '\0';
            memcpy(ap[i].bssid, bssid, 6);
            ap[i].chan = chan;
            if(packet[0] == 0x50) ap[i].found++;
            return 0;
        }
    }
    return -1;
}