int do_attack_test()
{
    unsigned char packet[4096];
    struct timeval tv, tv2, tv3;
    int len=0, i=0, j=0, k=0;
    int gotit=0, answers=0, found=0;
    int caplen=0, essidlen=0;
    unsigned int min, avg, max;
    int ret=0;
    float avg2;
    struct rx_info ri;
    int atime=200;  //time in ms to wait for answer packet (needs to be higher for airserv)
    unsigned char nulldata[1024];

    if(opt.port_out > 0)
    {
        atime += 200;
        PCT; printf("Testing connection to injection device %s\n", opt.iface_out);
        ret = tcp_test(opt.ip_out, opt.port_out);
        if(ret != 0)
        {
            return( 1 );
        }
        printf("\n");

        /* open the replay interface */
        _wi_out = wi_open(opt.iface_out);
        if (!_wi_out)
            return 1;
        printf("\n");
        dev.fd_out = wi_fd(_wi_out);
        wi_get_mac(_wi_out, dev.mac_out);
        if(opt.s_face == NULL)
        {
            _wi_in = _wi_out;
            dev.fd_in = dev.fd_out;

            /* XXX */
            dev.arptype_in = dev.arptype_out;
            wi_get_mac(_wi_in, dev.mac_in);
        }
    }

    if(opt.s_face && opt.port_in > 0)
    {
        atime += 200;
        PCT; printf("Testing connection to capture device %s\n", opt.s_face);
        ret = tcp_test(opt.ip_in, opt.port_in);
        if(ret != 0)
        {
            return( 1 );
        }
        printf("\n");

        /* open the packet source */
        _wi_in = wi_open(opt.s_face);
        if (!_wi_in)
            return 1;
        dev.fd_in = wi_fd(_wi_in);
        wi_get_mac(_wi_in, dev.mac_in);
        printf("\n");
    }
    else if(opt.s_face && opt.port_in <= 0)
    {
        _wi_in = wi_open(opt.s_face);
        if (!_wi_in)
            return 1;
        dev.fd_in = wi_fd(_wi_in);
        wi_get_mac(_wi_in, dev.mac_in);
        printf("\n");
    }

    if(opt.port_in <= 0)
    {
        /* avoid blocking on reading the socket */
        if( fcntl( dev.fd_in, F_SETFL, O_NONBLOCK ) < 0 )
        {
            perror( "fcntl(O_NONBLOCK) failed" );
            return( 1 );
        }
    }

    if(getnet(NULL, 0, 0) != 0)
        return 1;

    srand( time( NULL ) );

    memset(ap, '\0', 20*sizeof(struct APt));

    essidlen = strlen(opt.r_essid);
    if( essidlen > 250) essidlen = 250;

    if( essidlen > 0 )
    {
        ap[0].set = 1;
        ap[0].found = 0;
        ap[0].len = essidlen;
        memcpy(ap[0].essid, opt.r_essid, essidlen);
        ap[0].essid[essidlen] = '\0';
        memcpy(ap[0].bssid, opt.r_bssid, 6);
        found++;
    }

    if(opt.bittest)
        set_bitrate(_wi_out, RATE_1M);

    PCT; printf("Trying broadcast probe requests...\n");

    memcpy(h80211, PROBE_REQ, 24);

    len = 24;

    h80211[24] = 0x00;      //ESSID Tag Number
    h80211[25] = 0x00;      //ESSID Tag Length

    len += 2;

    memcpy(h80211+len, RATES, 16);

    len += 16;

    gotit=0;
    answers=0;
    for(i=0; i<3; i++)
    {
        /*
            random source so we can identify our packets
        */
        opt.r_smac[0] = 0x00;
        opt.r_smac[1] = rand() & 0xFF;
        opt.r_smac[2] = rand() & 0xFF;
        opt.r_smac[3] = rand() & 0xFF;
        opt.r_smac[4] = rand() & 0xFF;
        opt.r_smac[5] = rand() & 0xFF;

        memcpy(h80211+10, opt.r_smac, 6);

        send_packet(h80211, len);

        gettimeofday( &tv, NULL );

        while (1)  //waiting for relayed packet
        {
            caplen = read_packet(packet, sizeof(packet), &ri);

            if (packet[0] == 0x50 ) //Is probe response
            {
                if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                {
                    if(grab_essid(packet, caplen) == 0 && (!memcmp(opt.r_bssid, NULL_MAC, 6)))
                    {
                        found++;
                    }
                    if(!answers)
                    {
                        PCT; printf("Injection is working!\n");
                        if(opt.fast) return 0;
                        gotit=1;
                        answers++;
                    }
                }
            }

            if (packet[0] == 0x80 ) //Is beacon frame
            {
                if(grab_essid(packet, caplen) == 0 && (!memcmp(opt.r_bssid, NULL_MAC, 6)))
                {
                    found++;
                }
            }

            gettimeofday( &tv2, NULL );
            if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (3*atime*1000)) //wait 'atime'ms for an answer
            {
                break;
            }
        }
    }
    if(answers == 0)
    {
        PCT; printf("No Answer...\n");
    }

    PCT; printf("Found %d AP%c\n", found, ((found == 1) ? ' ' : 's' ) );

    if(found > 0)
    {
        printf("\n");
        PCT; printf("Trying directed probe requests...\n");
    }

    for(i=0; i<found; i++)
    {
        if(wi_get_channel(_wi_out) != ap[i].chan)
        {
            wi_set_channel(_wi_out, ap[i].chan);
        }

        if(wi_get_channel(_wi_in) != ap[i].chan)
        {
            wi_set_channel(_wi_in, ap[i].chan);
        }

        PCT; printf("%02X:%02X:%02X:%02X:%02X:%02X - channel: %d - \'%s\'\n", ap[i].bssid[0], ap[i].bssid[1],
                    ap[i].bssid[2], ap[i].bssid[3], ap[i].bssid[4], ap[i].bssid[5], ap[i].chan, ap[i].essid);

        ap[i].found=0;
        min = INT_MAX;
        max = 0;
        avg = 0;
        avg2 = 0;

        memcpy(h80211, PROBE_REQ, 24);

        len = 24;

        h80211[24] = 0x00;      //ESSID Tag Number
        h80211[25] = ap[i].len; //ESSID Tag Length
        memcpy(h80211+len+2, ap[i].essid, ap[i].len);

        len += ap[i].len+2;

        memcpy(h80211+len, RATES, 16);

        len += 16;

        for(j=0; j<REQUESTS; j++)
        {
            /*
                random source so we can identify our packets
            */
            opt.r_smac[0] = 0x00;
            opt.r_smac[1] = rand() & 0xFF;
            opt.r_smac[2] = rand() & 0xFF;
            opt.r_smac[3] = rand() & 0xFF;
            opt.r_smac[4] = rand() & 0xFF;
            opt.r_smac[5] = rand() & 0xFF;

            //build/send probe request
            memcpy(h80211+10, opt.r_smac, 6);

            send_packet(h80211, len);
            usleep(10);

            //build/send request-to-send
            memcpy(nulldata, RTS, 16);
            memcpy(nulldata+4, ap[i].bssid, 6);
            memcpy(nulldata+10, opt.r_smac, 6);

            send_packet(nulldata, 16);
            usleep(10);

            //build/send null data packet
            memcpy(nulldata, NULL_DATA, 24);
            memcpy(nulldata+4, ap[i].bssid, 6);
            memcpy(nulldata+10, opt.r_smac, 6);
            memcpy(nulldata+16, ap[i].bssid, 6);

            send_packet(nulldata, 24);
            usleep(10);

            //build/send auth request packet
            memcpy(nulldata, AUTH_REQ, 30);
            memcpy(nulldata+4, ap[i].bssid, 6);
            memcpy(nulldata+10, opt.r_smac, 6);
            memcpy(nulldata+16, ap[i].bssid, 6);

            send_packet(nulldata, 30);

            //continue
            gettimeofday( &tv, NULL );

            printf( "\r%2d/%2d: %3d%%\r", ap[i].found, j+1, ((ap[i].found*100)/(j+1)));
            fflush(stdout);
            while (1)  //waiting for relayed packet
            {
                caplen = read_packet(packet, sizeof(packet), &ri);

                if (packet[0] == 0x50 ) //Is probe response
                {
                    if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                    {
                        if(! memcmp(ap[i].bssid, packet+16, 6)) //From the mentioned AP
                        {
                            gettimeofday( &tv3, NULL);
                            ap[i].ping[j] = ((tv3.tv_sec*1000000 - tv.tv_sec*1000000) + (tv3.tv_usec - tv.tv_usec));
                            if(!answers)
                            {
                                if(opt.fast)
                                {
                                    PCT; printf("Injection is working!\n\n");
                                    return 0;
                                }
                                answers++;
                            }
                            ap[i].found++;
                            if((signed)ri.ri_power > -200)
                                ap[i].pwr[j] = (signed)ri.ri_power;
                            break;
                        }
                    }
                }

                if (packet[0] == 0xC4 ) //Is clear-to-send
                {
                    if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                    {
                        gettimeofday( &tv3, NULL);
                        ap[i].ping[j] = ((tv3.tv_sec*1000000 - tv.tv_sec*1000000) + (tv3.tv_usec - tv.tv_usec));
                        if(!answers)
                        {
                            if(opt.fast)
                            {
                                PCT; printf("Injection is working!\n\n");
                                return 0;
                            }
                            answers++;
                        }
                        ap[i].found++;
                        if((signed)ri.ri_power > -200)
                            ap[i].pwr[j] = (signed)ri.ri_power;
                        break;
                    }
                }

                if (packet[0] == 0xD4 ) //Is ack
                {
                    if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                    {
                        gettimeofday( &tv3, NULL);
                        ap[i].ping[j] = ((tv3.tv_sec*1000000 - tv.tv_sec*1000000) + (tv3.tv_usec - tv.tv_usec));
                        if(!answers)
                        {
                            if(opt.fast)
                            {
                                PCT; printf("Injection is working!\n\n");
                                return 0;
                            }
                            answers++;
                        }
                        ap[i].found++;
                        if((signed)ri.ri_power > -200)
                            ap[i].pwr[j] = (signed)ri.ri_power;
                        break;
                    }
                }

                if (packet[0] == 0xB0 ) //Is auth response
                {
                    if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                    {
                        if (! memcmp(packet+10, packet+16, 6)) //From BSS ID
                        {
                            gettimeofday( &tv3, NULL);
                            ap[i].ping[j] = ((tv3.tv_sec*1000000 - tv.tv_sec*1000000) + (tv3.tv_usec - tv.tv_usec));
                            if(!answers)
                            {
                                if(opt.fast)
                                {
                                    PCT; printf("Injection is working!\n\n");
                                    return 0;
                                }
                                answers++;
                            }
                            ap[i].found++;
                            if((signed)ri.ri_power > -200)
                                ap[i].pwr[j] = (signed)ri.ri_power;
                            break;
                        }
                    }
                }

                gettimeofday( &tv2, NULL );
                if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (atime*1000)) //wait 'atime'ms for an answer
                {
                    break;
                }
                usleep(10);
            }
            printf( "\r%2d/%2d: %3d%%\r", ap[i].found, j+1, ((ap[i].found*100)/(j+1)));
            fflush(stdout);
        }
        for(j=0; j<REQUESTS; j++)
        {
            if(ap[i].ping[j] > 0)
            {
                if(ap[i].ping[j] > max) max = ap[i].ping[j];
                if(ap[i].ping[j] < min) min = ap[i].ping[j];
                avg += ap[i].ping[j];
                avg2 += ap[i].pwr[j];
            }
        }
        if(ap[i].found > 0)
        {
            avg /= ap[i].found;
            avg2 /= ap[i].found;
            PCT; printf("Ping (min/avg/max): %.3fms/%.3fms/%.3fms Power: %.2f\n", (min/1000.0), (avg/1000.0), (max/1000.0), avg2);
        }
        PCT; printf("%2d/%2d: %3d%%\n\n", ap[i].found, REQUESTS, ((ap[i].found*100)/REQUESTS));

        if(!gotit && answers)
        {
            PCT; printf("Injection is working!\n\n");
            gotit=1;
        }
    }

    if(opt.bittest)
    {
        if(found > 0)
        {
            PCT; printf("Trying directed probe requests for all bitrates...\n");
        }

        for(i=0; i<found; i++)
        {
            if(ap[i].found <= 0)
                continue;
            printf("\n");
            PCT; printf("%02X:%02X:%02X:%02X:%02X:%02X - channel: %d - \'%s\'\n", ap[i].bssid[0], ap[i].bssid[1],
                        ap[i].bssid[2], ap[i].bssid[3], ap[i].bssid[4], ap[i].bssid[5], ap[i].chan, ap[i].essid);

            min = INT_MAX;
            max = 0;
            avg = 0;

            memcpy(h80211, PROBE_REQ, 24);

            len = 24;

            h80211[24] = 0x00;      //ESSID Tag Number
            h80211[25] = ap[i].len; //ESSID Tag Length
            memcpy(h80211+len+2, ap[i].essid, ap[i].len);

            len += ap[i].len+2;

            memcpy(h80211+len, RATES, 16);

            len += 16;

            for(k=0; k<RATE_NUM; k++)
            {
                ap[i].found=0;
                if(set_bitrate(_wi_out, bitrates[k]))
                    continue;


                avg2 = 0;
                memset(ap[i].pwr, 0, REQUESTS*sizeof(unsigned int));

                for(j=0; j<REQUESTS; j++)
                {
                    /*
                        random source so we can identify our packets
                    */
                    opt.r_smac[0] = 0x00;
                    opt.r_smac[1] = rand() & 0xFF;
                    opt.r_smac[2] = rand() & 0xFF;
                    opt.r_smac[3] = rand() & 0xFF;
                    opt.r_smac[4] = rand() & 0xFF;
                    opt.r_smac[5] = rand() & 0xFF;

                    memcpy(h80211+10, opt.r_smac, 6);

                    send_packet(h80211, len);

                    gettimeofday( &tv, NULL );

                    printf( "\r%2d/%2d: %3d%%\r", ap[i].found, j+1, ((ap[i].found*100)/(j+1)));
                    fflush(stdout);
                    while (1)  //waiting for relayed packet
                    {
                        caplen = read_packet(packet, sizeof(packet), &ri);

                        if (packet[0] == 0x50 ) //Is probe response
                        {
                            if (! memcmp(opt.r_smac, packet+4, 6)) //To our MAC
                            {
                                if(! memcmp(ap[i].bssid, packet+16, 6)) //From the mentioned AP
                                {
                                    if(!answers)
                                    {
                                        answers++;
                                    }
                                    ap[i].found++;
                                    if((signed)ri.ri_power > -200)
                                        ap[i].pwr[j] = (signed)ri.ri_power;
                                    break;
                                }
                            }
                        }

                        gettimeofday( &tv2, NULL );
                        if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (100*1000)) //wait 300ms for an answer
                        {
                            break;
                        }
                        usleep(10);
                    }
                    printf( "\r%2d/%2d: %3d%%\r", ap[i].found, j+1, ((ap[i].found*100)/(j+1)));
                    fflush(stdout);
                }
                for(j=0; j<REQUESTS; j++)
                    avg2 += ap[i].pwr[j];
                if(ap[i].found > 0)
                    avg2 /= ap[i].found;
                PCT; printf("Probing at %2.1f Mbps:\t%2d/%2d: %3d%%\n", wi_get_rate(_wi_out)/1000000.0,
                            ap[i].found, REQUESTS, ((ap[i].found*100)/REQUESTS));
            }

            if(!gotit && answers)
            {
                PCT; printf("Injection is working!\n\n");
                if(opt.fast) return 0;
                gotit=1;
            }
        }
    }
    if(opt.bittest)
        set_bitrate(_wi_out, RATE_1M);

    if( opt.s_face != NULL )
    {
        printf("\n");
        PCT; printf("Trying card-to-card injection...\n");

        /* sync both cards to the same channel, or the test will fail */
        if(wi_get_channel(_wi_out) != wi_get_channel(_wi_in))
        {
            wi_set_channel(_wi_out, wi_get_channel(_wi_in));
        }

        /* Attacks */
        for(i=0; i<5; i++)
        {
            k=0;
            /* random macs */
            opt.f_smac[0] = 0x00;
            opt.f_smac[1] = rand() & 0xFF;
            opt.f_smac[2] = rand() & 0xFF;
            opt.f_smac[3] = rand() & 0xFF;
            opt.f_smac[4] = rand() & 0xFF;
            opt.f_smac[5] = rand() & 0xFF;

            opt.f_dmac[0] = 0x00;
            opt.f_dmac[1] = rand() & 0xFF;
            opt.f_dmac[2] = rand() & 0xFF;
            opt.f_dmac[3] = rand() & 0xFF;
            opt.f_dmac[4] = rand() & 0xFF;
            opt.f_dmac[5] = rand() & 0xFF;

            opt.f_bssid[0] = 0x00;
            opt.f_bssid[1] = rand() & 0xFF;
            opt.f_bssid[2] = rand() & 0xFF;
            opt.f_bssid[3] = rand() & 0xFF;
            opt.f_bssid[4] = rand() & 0xFF;
            opt.f_bssid[5] = rand() & 0xFF;

            if(i==0) //attack -0
            {
                memcpy( h80211, DEAUTH_REQ, 26 );
                memcpy( h80211 + 16, opt.f_bssid, 6 );
                memcpy( h80211 +  4, opt.f_dmac,  6 );
                memcpy( h80211 + 10, opt.f_smac, 6 );

                opt.f_iswep = 0;
                opt.f_tods = 0; opt.f_fromds = 0;
                opt.f_minlen = opt.f_maxlen = 26;
            }
            else if(i==1) //attack -1 (open)
            {
                memcpy( h80211, AUTH_REQ, 30 );
                memcpy( h80211 +  4, opt.f_dmac, 6 );
                memcpy( h80211 + 10, opt.f_smac , 6 );
                memcpy( h80211 + 16, opt.f_bssid, 6 );

                opt.f_iswep = 0;
                opt.f_tods = 0; opt.f_fromds = 0;
                opt.f_minlen = opt.f_maxlen = 30;
            }
            else if(i==2) //attack -1 (psk)
            {
                memcpy( h80211, ska_auth3, 24);
                memcpy( h80211 +  4, opt.f_dmac, 6);
                memcpy( h80211 + 10, opt.f_smac,  6);
                memcpy( h80211 + 16, opt.f_bssid, 6);

                //iv+idx
                h80211[24] = 0x86;
                h80211[25] = 0xD8;
                h80211[26] = 0x2E;
                h80211[27] = 0x00;

                //random bytes (as encrypted data)
                for(j=0; j<132; j++)
                    h80211[28+j] = rand() & 0xFF;

                opt.f_iswep = 1;
                opt.f_tods = 0; opt.f_fromds = 0;
                opt.f_minlen = opt.f_maxlen = 24+4+132;
            }
            else if(i==3) //attack -3
            {
                memcpy( h80211, NULL_DATA, 24);
                memcpy( h80211 +  4, opt.f_bssid, 6);
                memcpy( h80211 + 10, opt.f_smac,  6);
                memcpy( h80211 + 16, opt.f_dmac, 6);

                //iv+idx
                h80211[24] = 0x86;
                h80211[25] = 0xD8;
                h80211[26] = 0x2E;
                h80211[27] = 0x00;

                //random bytes (as encrypted data)
                for(j=0; j<132; j++)
                    h80211[28+j] = rand() & 0xFF;

                opt.f_iswep = -1;
                opt.f_tods = 1; opt.f_fromds = 0;
                opt.f_minlen = opt.f_maxlen = 24+4+132;
            }
            else if(i==4) //attack -5
            {
                memcpy( h80211, NULL_DATA, 24);
                memcpy( h80211 +  4, opt.f_bssid, 6);
                memcpy( h80211 + 10, opt.f_smac,  6);
                memcpy( h80211 + 16, opt.f_dmac, 6);

                h80211[1] |= 0x04;
                h80211[22] = 0x0A;
                h80211[23] = 0x00;

                //iv+idx
                h80211[24] = 0x86;
                h80211[25] = 0xD8;
                h80211[26] = 0x2E;
                h80211[27] = 0x00;

                //random bytes (as encrypted data)
                for(j=0; j<7; j++)
                    h80211[28+j] = rand() & 0xFF;

                opt.f_iswep = -1;
                opt.f_tods = 1; opt.f_fromds = 0;
                opt.f_minlen = opt.f_maxlen = 24+4+7;
            }

            for(j=0; (j<(REQUESTS/4) && !k); j++) //try it 5 times
            {
                send_packet( h80211, opt.f_minlen );

                gettimeofday( &tv, NULL );
                while (1)  //waiting for relayed packet
                {
                    caplen = read_packet(packet, sizeof(packet), &ri);
                    if ( filter_packet(packet, caplen) == 0 ) //got same length and same type
                    {
                        if(!answers)
                        {
                            answers++;
                        }

                        if(i == 0) //attack -0
                        {
                            if( h80211[0] == packet[0] )
                            {
                                k=1;
                                break;
                            }
                        }
                        else if(i==1) //attack -1 (open)
                        {
                            if( h80211[0] == packet[0] )
                            {
                                k=1;
                                break;
                            }
                        }
                        else if(i==2) //attack -1 (psk)
                        {
                            if( h80211[0] == packet[0] && memcmp(h80211+24, packet+24, caplen-24) == 0 )
                            {
                                k=1;
                                break;
                            }
                        }
                        else if(i==3) //attack -2/-3/-4/-6
                        {
                            if( h80211[0] == packet[0] && memcmp(h80211+24, packet+24, caplen-24) == 0 )
                            {
                                k=1;
                                break;
                            }
                        }
                        else if(i==4) //attack -5/-7
                        {
                            if( h80211[0] == packet[0] && memcmp(h80211+24, packet+24, caplen-24) == 0 )
                            {
                               if( (packet[1] & 0x04) && memcmp( h80211+22, packet+22, 2 ) == 0 )
                               {
                                    k=1;
                                    break;
                               }
                            }
                        }
                    }

                    gettimeofday( &tv2, NULL );
                    if (((tv2.tv_sec*1000000 - tv.tv_sec*1000000) + (tv2.tv_usec - tv.tv_usec)) > (3*atime*1000)) //wait 3*'atime' ms for an answer
                    {
                        break;
                    }
                    usleep(10);
                }
            }
            if(k)
            {
                k=0;
                if(i==0) //attack -0
                {
                    PCT; printf("Attack -0:           OK\n");
                }
                else if(i==1) //attack -1 (open)
                {
                    PCT; printf("Attack -1 (open):    OK\n");
                }
                else if(i==2) //attack -1 (psk)
                {
                    PCT; printf("Attack -1 (psk):     OK\n");
                }
                else if(i==3) //attack -3
                {
                    PCT; printf("Attack -2/-3/-4/-6:  OK\n");
                }
                else if(i==4) //attack -5
                {
                    PCT; printf("Attack -5/-7:        OK\n");
                }
            }
            else
            {
                if(i==0) //attack -0
                {
                    PCT; printf("Attack -0:           Failed\n");
                }
                else if(i==1) //attack -1 (open)
                {
                    PCT; printf("Attack -1 (open):    Failed\n");
                }
                else if(i==2) //attack -1 (psk)
                {
                    PCT; printf("Attack -1 (psk):     Failed\n");
                }
                else if(i==3) //attack -3
                {
                    PCT; printf("Attack -2/-3/-4/-6:  Failed\n");
                }
                else if(i==4) //attack -5
                {
                    PCT; printf("Attack -5/-7:        Failed\n");
                }
            }
        }

        if(!gotit && answers)
        {
            PCT; printf("Injection is working!\n");
            if(opt.fast) return 0;
            gotit=1;
        }
    }
    return 0;
}