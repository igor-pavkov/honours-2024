void send_fragments(unsigned char *packet, int packet_len, unsigned char *iv, unsigned char *keystream, int fragsize, int ska)
{
    int t, u;
    int data_size;
    unsigned char frag[32+fragsize];
    int pack_size;
    int header_size=24;

    data_size = packet_len-header_size;
    packet[23] = (rand() % 0xFF);

    for (t=0; t+=fragsize;)
    {

    //Copy header
        memcpy(frag, packet, header_size);

    //Copy IV + KeyIndex
        memcpy(frag+header_size, iv, 4);

    //Copy data
        if(fragsize <= packet_len-(header_size+t-fragsize))
            memcpy(frag+header_size+4, packet+header_size+t-fragsize, fragsize);
        else
            memcpy(frag+header_size+4, packet+header_size+t-fragsize, packet_len-(header_size+t-fragsize));

    //Make ToDS frame
        if(!ska)
        {
            frag[1] |= 1;
            frag[1] &= 253;
        }

    //Set fragment bit
        if (t< data_size) frag[1] |= 4;
        if (t>=data_size) frag[1] &= 251;

    //Fragment number
        frag[22] = 0;
        for (u=t; u-=fragsize;)
        {
            frag[22] += 1;
        }
//        frag[23] = 0;

    //Calculate packet length
        if(fragsize <= packet_len-(header_size+t-fragsize))
            pack_size = header_size + 4 + fragsize;
        else
            pack_size = header_size + 4 + (packet_len-(header_size+t-fragsize));

    //Add ICV
        add_icv(frag, pack_size, header_size + 4);
        pack_size += 4;

    //Encrypt
        xor_keystream(frag + header_size + 4, keystream, fragsize+4);

    //Send
        send_packet(frag, pack_size);
        if (t<data_size)usleep(100);

        if (t>=data_size) break;
    }

}