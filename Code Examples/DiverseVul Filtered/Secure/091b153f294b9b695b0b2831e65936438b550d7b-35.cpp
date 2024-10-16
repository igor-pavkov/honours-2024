void dump_packet(unsigned char* packet, int len)
{
    int i=0;

    for(i=0; i<len; i++)
    {
        if(i>0 && i%4 == 0)printf(" ");
        if(i>0 && i%16 == 0)printf("\n");
        printf("%02X ", packet[i]);
    }
    printf("\n\n");
}