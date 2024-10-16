int main( int argc, char *argv[] )
{
    int n, i, ret;

    /* check the arguments */

    memset( &opt, 0, sizeof( opt ) );
    memset( &dev, 0, sizeof( dev ) );

    opt.f_type    = -1; opt.f_subtype   = -1;
    opt.f_minlen  = -1; opt.f_maxlen    = -1;
    opt.f_tods    = -1; opt.f_fromds    = -1;
    opt.f_iswep   = -1; opt.ringbuffer  =  8;

    opt.a_mode    = -1; opt.r_fctrl     = -1;
    opt.ghost     =  0;
    opt.delay     = 15; opt.bittest     =  0;
    opt.fast      =  0; opt.r_smac_set  =  0;
    opt.npackets  =  1; opt.nodetect    =  0;
    opt.rtc       =  1; opt.f_retry	=  0;
    opt.reassoc   =  0;

/* XXX */
#if 0
#if defined(__FreeBSD__)
    /*
        check what is our FreeBSD version. injection works
        only on 7-CURRENT so abort if it's a lower version.
    */
    if( __FreeBSD_version < 700000 )
    {
        fprintf( stderr, "Aireplay-ng does not work on this "
            "release of FreeBSD.\n" );
        exit( 1 );
    }
#endif
#endif

    while( 1 )
    {
        int option_index = 0;

        static struct option long_options[] = {
            {"deauth",      1, 0, '0'},
            {"fakeauth",    1, 0, '1'},
            {"interactive", 0, 0, '2'},
            {"arpreplay",   0, 0, '3'},
            {"chopchop",    0, 0, '4'},
            {"fragment",    0, 0, '5'},
            {"caffe-latte", 0, 0, '6'},
            {"cfrag",       0, 0, '7'},
            {"test",        0, 0, '9'},
            {"help",        0, 0, 'H'},
            {"fast",        0, 0, 'F'},
            {"bittest",     0, 0, 'B'},
            {"migmode",     0, 0, '8'},
            {"ignore-negative-one", 0, &opt.ignore_negative_one, 1},
            {0,             0, 0,  0 }
        };

        int option = getopt_long( argc, argv,
                        "b:d:s:m:n:u:v:t:T:f:g:w:x:p:a:c:h:e:ji:r:k:l:y:o:q:Q0:1:23456789HFBDR",
                        long_options, &option_index );

        if( option < 0 ) break;

        switch( option )
        {
            case 0 :

                break;

            case ':' :

                printf("\"%s --help\" for help.\n", argv[0]);
                return( 1 );

            case '?' :

                printf("\"%s --help\" for help.\n", argv[0]);
                return( 1 );

            case 'b' :

                if( getmac( optarg, 1 ,opt.f_bssid ) != 0 )
                {
                    printf( "Invalid BSSID (AP MAC address).\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'd' :

                if( getmac( optarg, 1, opt.f_dmac ) != 0 )
                {
                    printf( "Invalid destination MAC address.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 's' :

                if( getmac( optarg, 1, opt.f_smac ) != 0 )
                {
                    printf( "Invalid source MAC address.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'm' :

                ret = sscanf( optarg, "%d", &opt.f_minlen );
                if( opt.f_minlen < 0 || ret != 1 )
                {
                    printf( "Invalid minimum length filter. [>=0]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'n' :

                ret = sscanf( optarg, "%d", &opt.f_maxlen );
                if( opt.f_maxlen < 0 || ret != 1 )
                {
                    printf( "Invalid maximum length filter. [>=0]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'u' :

                ret = sscanf( optarg, "%d", &opt.f_type );
                if( opt.f_type < 0 || opt.f_type > 3 || ret != 1 )
                {
                    printf( "Invalid type filter. [0-3]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'v' :

                ret = sscanf( optarg, "%d", &opt.f_subtype );
                if( opt.f_subtype < 0 || opt.f_subtype > 15 || ret != 1 )
                {
                    printf( "Invalid subtype filter. [0-15]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'T' :
		ret = sscanf(optarg, "%d", &opt.f_retry);
		if ((opt.f_retry < 1) || (opt.f_retry > 65535) || (ret != 1)) {
			printf("Invalid retry setting. [1-65535]\n");
			printf("\"%s --help\" for help.\n", argv[0]);
			return(1);
		}
		break;

            case 't' :

                ret = sscanf( optarg, "%d", &opt.f_tods );
                if(( opt.f_tods != 0 && opt.f_tods != 1 ) || ret != 1 )
                {
                    printf( "Invalid tods filter. [0,1]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'f' :

                ret = sscanf( optarg, "%d", &opt.f_fromds );
                if(( opt.f_fromds != 0 && opt.f_fromds != 1 ) || ret != 1 )
                {
                    printf( "Invalid fromds filter. [0,1]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'w' :

                ret = sscanf( optarg, "%d", &opt.f_iswep );
                if(( opt.f_iswep != 0 && opt.f_iswep != 1 ) || ret != 1 )
                {
                    printf( "Invalid wep filter. [0,1]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'x' :

                ret = sscanf( optarg, "%d", &opt.r_nbpps );
                if( opt.r_nbpps < 1 || opt.r_nbpps > 1024 || ret != 1 )
                {
                    printf( "Invalid number of packets per second. [1-1024]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'o' :

                ret = sscanf( optarg, "%d", &opt.npackets );
                if( opt.npackets < 0 || opt.npackets > 512 || ret != 1 )
                {
                    printf( "Invalid number of packets per burst. [0-512]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'q' :

                ret = sscanf( optarg, "%d", &opt.delay );
                if( opt.delay < 1 || opt.delay > 600 || ret != 1 )
                {
                    printf( "Invalid number of seconds. [1-600]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'Q' :

                opt.reassoc = 1;
                break;

            case 'p' :

                ret = sscanf( optarg, "%x", &opt.r_fctrl );
                if( opt.r_fctrl < 0 || opt.r_fctrl > 65535 || ret != 1 )
                {
                    printf( "Invalid frame control word. [0-65535]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'a' :

                if( getmac( optarg, 1, opt.r_bssid ) != 0 )
                {
                    printf( "Invalid AP MAC address.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'c' :

                if( getmac( optarg, 1, opt.r_dmac ) != 0 )
                {
                    printf( "Invalid destination MAC address.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'g' :

                ret = sscanf( optarg, "%d", &opt.ringbuffer );
                if( opt.ringbuffer < 1 || ret != 1 )
                {
                    printf( "Invalid replay ring buffer size. [>=1]\n");
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'h' :

                if( getmac( optarg, 1, opt.r_smac ) != 0 )
                {
                    printf( "Invalid source MAC address.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.r_smac_set=1;
                break;

            case 'e' :

                memset(  opt.r_essid, 0, sizeof( opt.r_essid ) );
                strncpy( opt.r_essid, optarg, sizeof( opt.r_essid )  - 1 );
                break;

            case 'j' :

                opt.r_fromdsinj = 1;
                break;

            case 'D' :

                opt.nodetect = 1;
                break;

            case 'k' :

                inet_aton( optarg, (struct in_addr *) opt.r_dip );
                break;

            case 'l' :

                inet_aton( optarg, (struct in_addr *) opt.r_sip );
                break;

            case 'y' :

                if( opt.prga != NULL )
                {
                    printf( "PRGA file already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                if( read_prga(&(opt.prga), optarg) != 0 )
                {
                    return( 1 );
                }
                break;

            case 'i' :

                if( opt.s_face != NULL || opt.s_file )
                {
                    printf( "Packet source already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.s_face = optarg;
                opt.port_in = get_ip_port(opt.s_face, opt.ip_in, sizeof(opt.ip_in)-1);
                break;

            case 'r' :

                if( opt.s_face != NULL || opt.s_file )
                {
                    printf( "Packet source already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.s_file = optarg;
                break;

            case 'z' :

                opt.ghost = 1;

                break;

            case '0' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 0;

                for (i=0; optarg[i] != 0; i++)
                {
                    if (isdigit((int)optarg[i]) == 0)
                        break;
                }

                ret = sscanf( optarg, "%d", &opt.a_count );
                if( opt.a_count < 0 || optarg[i] != 0 || ret != 1)
                {
                    printf( "Invalid deauthentication count or missing value. [>=0]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case '1' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 1;

                for (i=0; optarg[i] != 0; i++)
                {
                    if (isdigit((int)optarg[i]) == 0)
                        break;
                }

                ret = sscanf( optarg, "%d", &opt.a_delay );
                if( opt.a_delay < 0 || optarg[i] != 0 || ret != 1)
                {
                    printf( "Invalid reauthentication delay or missing value. [>=0]\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case '2' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 2;
                break;

            case '3' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 3;
                break;

            case '4' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 4;
                break;

            case '5' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 5;
                break;

            case '6' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 6;
                break;

            case '7' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 7;
                break;

            case '9' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 9;
                break;

            case '8' :

                if( opt.a_mode != -1 )
                {
                    printf( "Attack mode already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                opt.a_mode = 8;
                break;

            case 'F' :

                opt.fast = 1;
                break;

            case 'B' :

                opt.bittest = 1;
                break;

            case 'H' :

                printf( usage, getVersion("Aireplay-ng", _MAJ, _MIN, _SUB_MIN, _REVISION, _BETA, _RC)  );
                return( 1 );

            case 'R' :

                opt.rtc = 0;
                break;

            default : goto usage;
        }
    }

    if( argc - optind != 1 )
    {
    	if(argc == 1)
    	{
usage:
	        printf( usage, getVersion("Aireplay-ng", _MAJ, _MIN, _SUB_MIN, _REVISION, _BETA, _RC)  );
        }
	    if( argc - optind == 0)
	    {
	    	printf("No replay interface specified.\n");
	    }
	    if(argc > 1)
	    {
    		printf("\"%s --help\" for help.\n", argv[0]);
	    }
        return( 1 );
    }

    if( opt.a_mode == -1 )
    {
        printf( "Please specify an attack mode.\n" );
   		printf("\"%s --help\" for help.\n", argv[0]);
        return( 1 );
    }

    if( (opt.f_minlen > 0 && opt.f_maxlen > 0) && opt.f_minlen > opt.f_maxlen )
    {
        printf( "Invalid length filter (min(-m):%d > max(-n):%d).\n",
                opt.f_minlen, opt.f_maxlen );
  		printf("\"%s --help\" for help.\n", argv[0]);
        return( 1 );
    }

    if ( opt.f_tods == 1 && opt.f_fromds == 1 )
    {
        printf( "FromDS and ToDS bit are set: packet has to come from the AP and go to the AP\n" );
    }

    dev.fd_rtc = -1;

    /* open the RTC device if necessary */

#if defined(__i386__)
#if defined(linux)
    if( opt.a_mode > 1 )
    {
        if( ( dev.fd_rtc = open( "/dev/rtc0", O_RDONLY ) ) < 0 )
        {
            dev.fd_rtc = 0;
        }

        if( (dev.fd_rtc == 0) && ( ( dev.fd_rtc = open( "/dev/rtc", O_RDONLY ) ) < 0 ) )
        {
            dev.fd_rtc = 0;
        }
        if(opt.rtc == 0)
        {
            dev.fd_rtc = -1;
        }
        if(dev.fd_rtc > 0)
        {
            if( ioctl( dev.fd_rtc, RTC_IRQP_SET, RTC_RESOLUTION ) < 0 )
            {
                perror( "ioctl(RTC_IRQP_SET) failed" );
                printf(
    "Make sure enhanced rtc device support is enabled in the kernel (module\n"
    "rtc, not genrtc) - also try 'echo 1024 >/proc/sys/dev/rtc/max-user-freq'.\n" );
                close( dev.fd_rtc );
                dev.fd_rtc = -1;
            }
            else
            {
                if( ioctl( dev.fd_rtc, RTC_PIE_ON, 0 ) < 0 )
                {
                    perror( "ioctl(RTC_PIE_ON) failed" );
                    close( dev.fd_rtc );
                    dev.fd_rtc = -1;
                }
            }
        }
        else
        {
            printf( "For information, no action required:"
                    " Using gettimeofday() instead of /dev/rtc\n" );
            dev.fd_rtc = -1;
        }

    }
#endif /* linux */
#endif /* i386 */

    opt.iface_out = argv[optind];
    opt.port_out = get_ip_port(opt.iface_out, opt.ip_out, sizeof(opt.ip_out)-1);

    //don't open interface(s) when using test mode and airserv
    if( ! (opt.a_mode == 9 && opt.port_out >= 0 ) )
    {
        /* open the replay interface */
        _wi_out = wi_open(opt.iface_out);
        if (!_wi_out)
            return 1;
        dev.fd_out = wi_fd(_wi_out);

        /* open the packet source */
        if( opt.s_face != NULL )
        {
            //don't open interface(s) when using test mode and airserv
            if( ! (opt.a_mode == 9 && opt.port_in >= 0 ) )
            {
                _wi_in = wi_open(opt.s_face);
                if (!_wi_in)
                    return 1;
                dev.fd_in = wi_fd(_wi_in);
                wi_get_mac(_wi_in, dev.mac_in);
            }
        }
        else
        {
            _wi_in = _wi_out;
            dev.fd_in = dev.fd_out;

            /* XXX */
            dev.arptype_in = dev.arptype_out;
            wi_get_mac(_wi_in, dev.mac_in);
        }

        wi_get_mac(_wi_out, dev.mac_out);
    }

    /* drop privileges */
    if (setuid( getuid() ) == -1) {
		perror("setuid");
	}

    /* XXX */
    if( opt.r_nbpps == 0 )
    {
        if( dev.is_wlanng || dev.is_hostap )
            opt.r_nbpps = 200;
        else
            opt.r_nbpps = 500;
    }


    if( opt.s_file != NULL )
    {
        if( ! ( dev.f_cap_in = fopen( opt.s_file, "rb" ) ) )
        {
            perror( "open failed" );
            return( 1 );
        }

        n = sizeof( struct pcap_file_header );

        if( fread( &dev.pfh_in, 1, n, dev.f_cap_in ) != (size_t) n )
        {
            perror( "fread(pcap file header) failed" );
            return( 1 );
        }

        if( dev.pfh_in.magic != TCPDUMP_MAGIC &&
            dev.pfh_in.magic != TCPDUMP_CIGAM )
        {
            fprintf( stderr, "\"%s\" isn't a pcap file (expected "
                             "TCPDUMP_MAGIC).\n", opt.s_file );
            return( 1 );
        }

        if( dev.pfh_in.magic == TCPDUMP_CIGAM )
            SWAP32(dev.pfh_in.linktype);

        if( dev.pfh_in.linktype != LINKTYPE_IEEE802_11 &&
            dev.pfh_in.linktype != LINKTYPE_PRISM_HEADER &&
            dev.pfh_in.linktype != LINKTYPE_RADIOTAP_HDR &&
            dev.pfh_in.linktype != LINKTYPE_PPI_HDR )
        {
            fprintf( stderr, "Wrong linktype from pcap file header "
                             "(expected LINKTYPE_IEEE802_11) -\n"
                             "this doesn't look like a regular 802.11 "
                             "capture.\n" );
            return( 1 );
        }
    }

    //if there is no -h given, use default hardware mac
    if( maccmp( opt.r_smac, NULL_MAC) == 0 )
    {
        memcpy( opt.r_smac, dev.mac_out, 6);
        if(opt.a_mode != 0 && opt.a_mode != 4 && opt.a_mode != 9)
        {
            printf("No source MAC (-h) specified. Using the device MAC (%02X:%02X:%02X:%02X:%02X:%02X)\n",
                    dev.mac_out[0], dev.mac_out[1], dev.mac_out[2], dev.mac_out[3], dev.mac_out[4], dev.mac_out[5]);
        }
    }

    if( maccmp( opt.r_smac, dev.mac_out) != 0 && maccmp( opt.r_smac, NULL_MAC) != 0)
    {
//        if( dev.is_madwifi && opt.a_mode == 5 ) printf("For --fragment to work on madwifi[-ng], set the interface MAC according to (-h)!\n");
        fprintf( stderr, "The interface MAC (%02X:%02X:%02X:%02X:%02X:%02X)"
                 " doesn't match the specified MAC (-h).\n"
                 "\tifconfig %s hw ether %02X:%02X:%02X:%02X:%02X:%02X\n",
                 dev.mac_out[0], dev.mac_out[1], dev.mac_out[2], dev.mac_out[3], dev.mac_out[4], dev.mac_out[5],
                 opt.iface_out, opt.r_smac[0], opt.r_smac[1], opt.r_smac[2], opt.r_smac[3], opt.r_smac[4], opt.r_smac[5] );
    }

    switch( opt.a_mode )
    {
        case 0 : return( do_attack_deauth()      );
        case 1 : return( do_attack_fake_auth()   );
        case 2 : return( do_attack_interactive() );
        case 3 : return( do_attack_arp_resend()  );
        case 4 : return( do_attack_chopchop()    );
        case 5 : return( do_attack_fragment()    );
        case 6 : return( do_attack_caffe_latte() );
        case 7 : return( do_attack_cfrag()       );
        case 8 : return( do_attack_migmode()     );
        case 9 : return( do_attack_test()        );
        default: break;
    }

    /* that's all, folks */

    return( 0 );
}