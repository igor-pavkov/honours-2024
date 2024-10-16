int main( int argc, char *argv[] )
{
    long time_slept, cycle_time, cycle_time2;
    char * output_format_string;
    int caplen=0, i, j, fdh, fd_is_set, chan_count, freq_count, unused;
    int fd_raw[MAX_CARDS], arptype[MAX_CARDS];
    int ivs_only, found;
    int valid_channel;
    int freq [2];
    int num_opts = 0;
    int option = 0;
    int option_index = 0;
    char ifnam[64];
    int wi_read_failed=0;
    int n = 0;
    int output_format_first_time = 1;
#ifdef HAVE_PCRE
    const char *pcreerror;
    int pcreerroffset;
#endif

    struct AP_info *ap_cur, *ap_prv, *ap_next;
    struct ST_info *st_cur, *st_next;
    struct NA_info *na_cur, *na_next;
    struct oui *oui_cur, *oui_next;

    struct pcap_pkthdr pkh;

    time_t tt1, tt2, tt3, start_time;

    struct wif	       *wi[MAX_CARDS];
    struct rx_info     ri;
    unsigned char      tmpbuf[4096];
    unsigned char      buffer[4096];
    unsigned char      *h80211;
    char               *iface[MAX_CARDS];

    struct timeval     tv0;
    struct timeval     tv1;
    struct timeval     tv2;
    struct timeval     tv3;
    struct timeval     tv4;
    struct tm          *lt;

    /*
    struct sockaddr_in provis_addr;
    */

    fd_set             rfds;

    static struct option long_options[] = {
        {"band",     1, 0, 'b'},
        {"beacon",   0, 0, 'e'},
        {"beacons",  0, 0, 'e'},
        {"cswitch",  1, 0, 's'},
        {"netmask",  1, 0, 'm'},
        {"bssid",    1, 0, 'd'},
        {"essid",    1, 0, 'N'},
        {"essid-regex", 1, 0, 'R'},
        {"channel",  1, 0, 'c'},
        {"gpsd",     0, 0, 'g'},
        {"ivs",      0, 0, 'i'},
        {"write",    1, 0, 'w'},
        {"encrypt",  1, 0, 't'},
        {"update",   1, 0, 'u'},
        {"berlin",   1, 0, 'B'},
        {"help",     0, 0, 'H'},
        {"nodecloak",0, 0, 'D'},
        {"showack",  0, 0, 'A'},
        {"detect-anomaly", 0, 0, 'E'},
        {"output-format",  1, 0, 'o'},
        {"ignore-negative-one", 0, &G.ignore_negative_one, 1},
        {"manufacturer",  0, 0, 'M'},
        {"uptime",   0, 0, 'U'},
        {0,          0, 0,  0 }
    };


#ifdef USE_GCRYPT
    // Register callback functions to ensure proper locking in the sensitive parts of libgcrypt.
    gcry_control (GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
    // Disable secure memory.
    gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
    // Tell Libgcrypt that initialization has completed.
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
#endif
	pthread_mutex_init( &(G.mx_print), NULL );
    pthread_mutex_init( &(G.mx_sort), NULL );

    textstyle(TEXT_RESET);//(TEXT_RESET, TEXT_BLACK, TEXT_WHITE);

	/* initialize a bunch of variables */

	srand( time( NULL ) );
    memset( &G, 0, sizeof( G ) );

    h80211         =  NULL;
    ivs_only       =  0;
    G.chanoption   =  0;
    G.freqoption   =  0;
    G.num_cards	   =  0;
    fdh		   =  0;
    fd_is_set	   =  0;
    chan_count	   =  0;
    time_slept     =  0;
    G.batt         =  NULL;
    G.chswitch     =  0;
    valid_channel  =  0;
    G.usegpsd      =  0;
    G.channels     =  bg_chans;
    G.one_beacon   =  1;
    G.singlechan   =  0;
    G.singlefreq   =  0;
    G.dump_prefix  =  NULL;
    G.record_data  =  0;
    G.f_cap        =  NULL;
    G.f_ivs        =  NULL;
    G.f_txt        =  NULL;
    G.f_kis        =  NULL;
    G.f_kis_xml    =  NULL;
    G.f_gps        =  NULL;
    G.keyout       =  NULL;
    G.f_xor        =  NULL;
    G.sk_len       =  0;
    G.sk_len2      =  0;
    G.sk_start     =  0;
    G.prefix       =  NULL;
    G.f_encrypt    =  0;
    G.asso_client  =  0;
    G.f_essid      =  NULL;
    G.f_essid_count = 0;
    G.active_scan_sim  =  0;
    G.update_s     =  0;
    G.decloak      =  1;
    G.is_berlin    =  0;
    G.numaps       =  0;
    G.maxnumaps    =  0;
    G.berlin       =  120;
    G.show_ap      =  1;
    G.show_sta     =  1;
    G.show_ack     =  0;
    G.hide_known   =  0;
    G.maxsize_essid_seen  =  5; // Initial value: length of "ESSID"
    G.show_manufacturer = 0;
    G.show_uptime  = 0;
    G.hopfreq      =  DEFAULT_HOPFREQ;
    G.s_file       =  NULL;
    G.s_iface      =  NULL;
    G.f_cap_in     =  NULL;
    G.detect_anomaly = 0;
    G.airodump_start_time = NULL;
	G.manufList = NULL;

	G.output_format_pcap = 1;
    G.output_format_csv = 1;
    G.output_format_kismet_csv = 1;
    G.output_format_kismet_netxml = 1;

#ifdef HAVE_PCRE
    G.f_essid_regex = NULL;
#endif

	// Default selection.
    resetSelection();

    memset(G.sharedkey, '\x00', 512*3);
    memset(G.message, '\x00', sizeof(G.message));
    memset(&G.pfh_in, '\x00', sizeof(struct pcap_file_header));

    gettimeofday( &tv0, NULL );

    lt = localtime( (time_t *) &tv0.tv_sec );

    G.keyout = (char*) malloc(512);
    memset( G.keyout, 0, 512 );
    snprintf( G.keyout,  511,
              "keyout-%02d%02d-%02d%02d%02d.keys",
              lt->tm_mon + 1, lt->tm_mday,
              lt->tm_hour, lt->tm_min, lt->tm_sec );

    for(i=0; i<MAX_CARDS; i++)
    {
        arptype[i]=0;
        fd_raw[i]=-1;
        G.channel[i]=0;
    }

    memset(G.f_bssid, '\x00', 6);
    memset(G.f_netmask, '\x00', 6);
    memset(G.wpa_bssid, '\x00', 6);


    /* check the arguments */

    for(i=0; long_options[i].name != NULL; i++);
    num_opts = i;

    for(i=0; i<argc; i++) //go through all arguments
    {
        found = 0;
        if(strlen(argv[i]) >= 3)
        {
            if(argv[i][0] == '-' && argv[i][1] != '-')
            {
                //we got a single dash followed by at least 2 chars
                //lets check that against our long options to find errors
                for(j=0; j<num_opts;j++)
                {
                    if( strcmp(argv[i]+1, long_options[j].name) == 0 )
                    {
                        //found long option after single dash
                        found = 1;
                        if(i>1 && strcmp(argv[i-1], "-") == 0)
                        {
                            //separated dashes?
                            printf("Notice: You specified \"%s %s\". Did you mean \"%s%s\" instead?\n", argv[i-1], argv[i], argv[i-1], argv[i]);
                        }
                        else
                        {
                            //forgot second dash?
                            printf("Notice: You specified \"%s\". Did you mean \"-%s\" instead?\n", argv[i], argv[i]);
                        }
                        break;
                    }
                }
                if(found)
                {
                    sleep(3);
                    break;
                }
            }
        }
    }

    do
    {
        option_index = 0;

        option = getopt_long( argc, argv,
                        "b:c:egiw:s:t:u:m:d:N:R:aHDB:Ahf:r:EC:o:x:MU",
                        long_options, &option_index );

        if( option < 0 ) break;

        switch( option )
        {
            case 0 :

                break;

            case ':':

                printf("\"%s --help\" for help.\n", argv[0]);
                return( 1 );

            case '?':

                printf("\"%s --help\" for help.\n", argv[0]);
                return( 1 );

			case 'E':
				G.detect_anomaly = 1;
				break;

            case 'e':

                G.one_beacon = 0;
                break;

            case 'a':

                G.asso_client = 1;
                break;

            case 'A':

                G.show_ack = 1;
                break;

            case 'h':

                G.hide_known = 1;
                break;

            case 'D':

                G.decloak = 0;
                break;

	    case 'M':

                G.show_manufacturer = 1;
                break;

	    case 'U' :
	    		G.show_uptime = 1;
	    		break;

            case 'c' :

                if (G.channel[0] > 0 || G.chanoption == 1) {
                    if (G.chanoption == 1)
                        printf( "Notice: Channel range already given\n" );
                    else
                        printf( "Notice: Channel already given (%d)\n", G.channel[0]);
                    break;
                }

                G.channel[0] = getchannels(optarg);

                if ( G.channel[0] < 0 )
                    goto usage;

                G.chanoption = 1;

                if( G.channel[0] == 0 )
                {
                    G.channels = G.own_channels;
                    break;
                }
                G.channels = bg_chans;
                break;

            case 'C' :

                if (G.channel[0] > 0 || G.chanoption == 1) {
                    if (G.chanoption == 1)
                        printf( "Notice: Channel range already given\n" );
                    else
                        printf( "Notice: Channel already given (%d)\n", G.channel[0]);
                    break;
                }

                if (G.freqoption == 1) {
                    printf( "Notice: Frequency range already given\n" );
                    break;
                }

                G.freqstring = optarg;

                G.freqoption = 1;

                break;

            case 'b' :

                if (G.chanoption == 1 && option != 'c') {
                    printf( "Notice: Channel range already given\n" );
                    break;
                }
                freq[0] = freq[1] = 0;

                for (i = 0; i < (int)strlen(optarg); i++) {
                    if ( optarg[i] == 'a' )
                        freq[1] = 1;
                    else if ( optarg[i] == 'b' || optarg[i] == 'g')
                        freq[0] = 1;
                    else {
                        printf( "Error: invalid band (%c)\n", optarg[i] );
                        printf("\"%s --help\" for help.\n", argv[0]);
                        exit ( 1 );
                    }
                }

                if (freq[1] + freq[0] == 2 )
                    G.channels = abg_chans;
                else {
                    if ( freq[1] == 1 )
                        G.channels = a_chans;
                    else
                        G.channels = bg_chans;
                }

                break;

            case 'i':

				// Reset output format if it's the first time the option is specified
				if (output_format_first_time) {
					output_format_first_time = 0;

					G.output_format_pcap = 0;
					G.output_format_csv = 0;
					G.output_format_kismet_csv = 0;
    				G.output_format_kismet_netxml = 0;
				}

 				if (G.output_format_pcap) {
					printf( usage, getVersion("Airodump-ng", _MAJ, _MIN, _SUB_MIN, _REVISION, _BETA, _RC)  );
					fprintf(stderr, "Invalid output format: IVS and PCAP format cannot be used together.\n");
					return( 1 );
				}

                ivs_only = 1;
                break;

            case 'g':

                G.usegpsd  = 1;
                /*
                if (inet_aton(optarg, &provis_addr.sin_addr) == 0 )
                {
                    printf("Invalid IP address.\n");
                    return (1);
                }
                */
                break;

            case 'w':

                if (G.dump_prefix != NULL) {
                    printf( "Notice: dump prefix already given\n" );
                    break;
                }
                /* Write prefix */
                G.dump_prefix   = optarg;
                G.record_data = 1;
                break;

            case 'r' :

                if( G.s_file )
                {
                    printf( "Packet source already specified.\n" );
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                G.s_file = optarg;
                break;

            case 's':

                if (atoi(optarg) > 2) {
                    goto usage;
                }
                if (G.chswitch != 0) {
                    printf("Notice: switching method already given\n");
                    break;
                }
                G.chswitch = atoi(optarg);
                break;

            case 'u':

                G.update_s = atoi(optarg);

                /* If failed to parse or value <= 0, use default, 100ms */
                if (G.update_s <= 0)
                	G.update_s = REFRESH_RATE;

                break;

            case 'f':

                G.hopfreq = atoi(optarg);

                /* If failed to parse or value <= 0, use default, 100ms */
                if (G.hopfreq <= 0)
                	G.hopfreq = DEFAULT_HOPFREQ;

                break;

            case 'B':

                G.is_berlin = 1;
                G.berlin    = atoi(optarg);

                if (G.berlin <= 0)
                	G.berlin = 120;

                break;

            case 'm':

                if ( memcmp(G.f_netmask, NULL_MAC, 6) != 0 )
                {
                    printf("Notice: netmask already given\n");
                    break;
                }
                if(getmac(optarg, 1, G.f_netmask) != 0)
                {
                    printf("Notice: invalid netmask\n");
                    printf("\"%s --help\" for help.\n", argv[0]);
                    return( 1 );
                }
                break;

            case 'd':

                if ( memcmp(G.f_bssid, NULL_MAC, 6) != 0 )
                {
                    printf("Notice: bssid already given\n");
                    break;
                }
                if(getmac(optarg, 1, G.f_bssid) != 0)
                {
                    printf("Notice: invalid bssid\n");
                    printf("\"%s --help\" for help.\n", argv[0]);

                    return( 1 );
                }
                break;

            case 'N':

                G.f_essid_count++;
                G.f_essid = (char**)realloc(G.f_essid, G.f_essid_count * sizeof(char*));
                G.f_essid[G.f_essid_count-1] = optarg;
                break;

	    case 'R':

#ifdef HAVE_PCRE
                if (G.f_essid_regex != NULL)
                {
			printf("Error: ESSID regular expression already given. Aborting\n");
			exit(1);
                }

                G.f_essid_regex = pcre_compile(optarg, 0, &pcreerror, &pcreerroffset, NULL);

                if (G.f_essid_regex == NULL)
                {
			printf("Error: regular expression compilation failed at offset %d: %s; aborting\n", pcreerroffset, pcreerror);
			exit(1);
		}
#else
                printf("Error: Airodump-ng wasn't compiled with pcre support; aborting\n");
#endif

                break;

            case 't':

                set_encryption_filter(optarg);
                break;

			case 'o':

				// Reset output format if it's the first time the option is specified
				if (output_format_first_time) {
					output_format_first_time = 0;

					G.output_format_pcap = 0;
					G.output_format_csv = 0;
					G.output_format_kismet_csv = 0;
    				G.output_format_kismet_netxml = 0;
				}

				// Parse the value
				output_format_string = strtok(optarg, ",");
				while (output_format_string != NULL) {
					if (strlen(output_format_string) != 0) {
						if (strncasecmp(output_format_string, "csv", 3) == 0
							|| strncasecmp(output_format_string, "txt", 3) == 0) {
							G.output_format_csv = 1;
						} else if (strncasecmp(output_format_string, "pcap", 4) == 0
							|| strncasecmp(output_format_string, "cap", 3) == 0) {
                            if (ivs_only) {
                                printf( usage, getVersion("Airodump-ng", _MAJ, _MIN, _SUB_MIN, _REVISION, _BETA, _RC)  );
                                fprintf(stderr, "Invalid output format: IVS and PCAP format cannot be used together.\n");
                                return( 1 );
                            }
							G.output_format_pcap = 1;
						} else if (strncasecmp(output_format_string, "ivs", 3) == 0) {
                            if (G.output_format_pcap) {
                                printf( usage, getVersion("Airodump-ng", _MAJ, _MIN, _SUB_MIN, _REVISION, _BETA, _RC)  );
                                fprintf(stderr, "Invalid output format: IVS and PCAP format cannot be used together.\n");
                                return( 1 );
                            }
							ivs_only = 1;
						} else if (strncasecmp(output_format_string, "kismet", 6) == 0) {
							G.output_format_kismet_csv = 1;
						} else if (strncasecmp(output_format_string, "gps", 3) == 0) {
							G.usegpsd  = 1;
						} else if (strncasecmp(output_format_string, "netxml", 6) == 0
							|| strncasecmp(output_format_string, "newcore", 7) == 0
							|| strncasecmp(output_format_string, "kismet-nc", 9) == 0
							|| strncasecmp(output_format_string, "kismet_nc", 9) == 0
							|| strncasecmp(output_format_string, "kismet-newcore", 14) == 0
							|| strncasecmp(output_format_string, "kismet_newcore", 14) == 0) {
							G.output_format_kismet_netxml = 1;
						} else if (strncasecmp(output_format_string, "default", 6) == 0) {
							G.output_format_pcap = 1;
							G.output_format_csv = 1;
							G.output_format_kismet_csv = 1;
							G.output_format_kismet_netxml = 1;
						} else if (strncasecmp(output_format_string, "none", 6) == 0) {
							G.output_format_pcap = 0;
							G.output_format_csv = 0;
							G.output_format_kismet_csv = 0;
    						G.output_format_kismet_netxml = 0;

							G.usegpsd  = 0;
							ivs_only = 0;
						} else {
							// Display an error if it does not match any value
							fprintf(stderr, "Invalid output format: <%s>\n", output_format_string);
							exit(1);
						}
					}
					output_format_string = strtok(NULL, ",");
				}

				break;

            case 'H':

                printf( usage, getVersion("Airodump-ng", _MAJ, _MIN, _SUB_MIN, _REVISION, _BETA, _RC)  );
                return( 1 );

            case 'x':

                G.active_scan_sim = atoi(optarg);

                if (G.active_scan_sim <= 0)
                    G.active_scan_sim = 0;
                break;

            default : goto usage;
        }
    } while ( 1 );

    if( argc - optind != 1 && G.s_file == NULL)
    {
        if(argc == 1)
        {
usage:
            printf( usage, getVersion("Airodump-ng", _MAJ, _MIN, _SUB_MIN, _REVISION, _BETA, _RC)  );
        }
        if( argc - optind == 0)
        {
            printf("No interface specified.\n");
        }
        if(argc > 1)
        {
            printf("\"%s --help\" for help.\n", argv[0]);
        }
        return( 1 );
    }

    if( argc - optind == 1 )
        G.s_iface = argv[argc-1];

    if( ( memcmp(G.f_netmask, NULL_MAC, 6) != 0 ) && ( memcmp(G.f_bssid, NULL_MAC, 6) == 0 ) )
    {
        printf("Notice: specify bssid \"--bssid\" with \"--netmask\"\n");
        printf("\"%s --help\" for help.\n", argv[0]);
        return( 1 );
    }

    if(G.s_iface != NULL)
    {
        /* initialize cards */
        G.num_cards = init_cards(G.s_iface, iface, wi);

        if(G.num_cards <= 0)
            return( 1 );

        for (i = 0; i < G.num_cards; i++) {
            fd_raw[i] = wi_fd(wi[i]);
            if (fd_raw[i] > fdh)
                fdh = fd_raw[i];
        }

        if(G.freqoption == 1 && G.freqstring != NULL) // use frequencies
        {
            detect_frequencies(wi[0]);
            G.frequency[0] = getfrequencies(G.freqstring);
            if(G.frequency[0] == -1)
            {
                printf("No valid frequency given.\n");
                return(1);
            }

//             printf("gonna rearrange\n");
            rearrange_frequencies();
//             printf("finished rearranging\n");

            freq_count = getfreqcount(0);

            /* find the interface index */
            /* start a child to hop between frequencies */

            if( G.frequency[0] == 0 )
            {
                unused = pipe( G.ch_pipe );
                unused = pipe( G.cd_pipe );

                signal( SIGUSR1, sighandler );

                if( ! fork() )
                {
                    /* reopen cards.  This way parent & child don't share resources for
                    * accessing the card (e.g. file descriptors) which may cause
                    * problems.  -sorbo
                    */
                    for (i = 0; i < G.num_cards; i++) {
                        strncpy(ifnam, wi_get_ifname(wi[i]), sizeof(ifnam)-1);
                        ifnam[sizeof(ifnam)-1] = 0;

                        wi_close(wi[i]);
                        wi[i] = wi_open(ifnam);
                        if (!wi[i]) {
                                printf("Can't reopen %s\n", ifnam);
                                exit(1);
                        }
                    }

					/* Drop privileges */
					if (setuid( getuid() ) == -1) {
						perror("setuid");
					}

                    frequency_hopper(wi, G.num_cards, freq_count);
                    exit( 1 );
                }
            }
            else
            {
                for( i=0; i<G.num_cards; i++ )
                {
                    wi_set_freq(wi[i], G.frequency[0]);
                    G.frequency[i] = G.frequency[0];
                }
                G.singlefreq = 1;
            }
        }
        else    //use channels
        {
            chan_count = getchancount(0);

            /* find the interface index */
            /* start a child to hop between channels */

            if( G.channel[0] == 0 )
            {
                unused = pipe( G.ch_pipe );
                unused = pipe( G.cd_pipe );

                signal( SIGUSR1, sighandler );

                if( ! fork() )
                {
                    /* reopen cards.  This way parent & child don't share resources for
                    * accessing the card (e.g. file descriptors) which may cause
                    * problems.  -sorbo
                    */
                    for (i = 0; i < G.num_cards; i++) {
                        strncpy(ifnam, wi_get_ifname(wi[i]), sizeof(ifnam)-1);
                        ifnam[sizeof(ifnam)-1] = 0;

                        wi_close(wi[i]);
                        wi[i] = wi_open(ifnam);
                        if (!wi[i]) {
                                printf("Can't reopen %s\n", ifnam);
                                exit(1);
                        }
                    }

					/* Drop privileges */
					if (setuid( getuid() ) == -1) {
						perror("setuid");
					}

                    channel_hopper(wi, G.num_cards, chan_count);
                    exit( 1 );
                }
            }
            else
            {
                for( i=0; i<G.num_cards; i++ )
                {
                    wi_set_channel(wi[i], G.channel[0]);
                    G.channel[i] = G.channel[0];
                }
                G.singlechan = 1;
            }
        }
    }

	/* Drop privileges */
	if (setuid( getuid() ) == -1) {
		perror("setuid");
	}

    /* check if there is an input file */
    if( G.s_file != NULL )
    {
        if( ! ( G.f_cap_in = fopen( G.s_file, "rb" ) ) )
        {
            perror( "open failed" );
            return( 1 );
        }

        n = sizeof( struct pcap_file_header );

        if( fread( &G.pfh_in, 1, n, G.f_cap_in ) != (size_t) n )
        {
            perror( "fread(pcap file header) failed" );
            return( 1 );
        }

        if( G.pfh_in.magic != TCPDUMP_MAGIC &&
            G.pfh_in.magic != TCPDUMP_CIGAM )
        {
            fprintf( stderr, "\"%s\" isn't a pcap file (expected "
                             "TCPDUMP_MAGIC).\n", G.s_file );
            return( 1 );
        }

        if( G.pfh_in.magic == TCPDUMP_CIGAM )
            SWAP32(G.pfh_in.linktype);

        if( G.pfh_in.linktype != LINKTYPE_IEEE802_11 &&
            G.pfh_in.linktype != LINKTYPE_PRISM_HEADER &&
            G.pfh_in.linktype != LINKTYPE_RADIOTAP_HDR &&
            G.pfh_in.linktype != LINKTYPE_PPI_HDR )
        {
            fprintf( stderr, "Wrong linktype from pcap file header "
                             "(expected LINKTYPE_IEEE802_11) -\n"
                             "this doesn't look like a regular 802.11 "
                             "capture.\n" );
            return( 1 );
        }
    }

    /* open or create the output files */

    if (G.record_data)
    	if( dump_initialize( G.dump_prefix, ivs_only ) )
    	    return( 1 );

    signal( SIGINT,   sighandler );
    signal( SIGSEGV,  sighandler );
    signal( SIGTERM,  sighandler );
    signal( SIGWINCH, sighandler );

    sighandler( SIGWINCH );

    /* fill oui struct if ram is greater than 32 MB */
    if (get_ram_size()  > MIN_RAM_SIZE_LOAD_OUI_RAM) {
        G.manufList = load_oui_file();
	}

    /* start the GPS tracker */

    if (G.usegpsd)
    {
        unused = pipe( G.gc_pipe );
        signal( SIGUSR2, sighandler );

        if( ! fork() )
        {
            gps_tracker();
            exit( 1 );
        }

        usleep( 50000 );
        waitpid( -1, NULL, WNOHANG );
    }

    fprintf( stderr, "\33[?25l\33[2J\n" );

    start_time = time( NULL );
    tt1        = time( NULL );
    tt2        = time( NULL );
    tt3        = time( NULL );
    gettimeofday( &tv3, NULL );
    gettimeofday( &tv4, NULL );

    G.batt     = getBatteryString();

    G.elapsed_time = (char *) calloc( 1, 4 );
    strncpy(G.elapsed_time, "0 s", 4 - 1);

	/* Create start time string for kismet netxml file */
    G.airodump_start_time = (char *) calloc( 1, 1000 * sizeof(char) );
    strncpy(G.airodump_start_time, ctime( & start_time ), 1000 - 1);
    G.airodump_start_time[strlen(G.airodump_start_time) - 1] = 0; // remove new line
    G.airodump_start_time = (char *) realloc( G.airodump_start_time, sizeof(char) * (strlen(G.airodump_start_time) + 1) );

    if( pthread_create( &(G.input_tid), NULL, (void *) input_thread, NULL ) != 0 )
    {
	perror( "pthread_create failed" );
	return 1;
    }


    while( 1 )
    {
        if( G.do_exit )
        {
            break;
        }

        if( time( NULL ) - tt1 >= 5 )
        {
            /* update the csv stats file */

            tt1 = time( NULL );
            if (G. output_format_csv)  dump_write_csv();
            if (G.output_format_kismet_csv) dump_write_kismet_csv();
            if (G.output_format_kismet_netxml) dump_write_kismet_netxml();

            /* sort the APs by power */

	    if(G.sort_by != SORT_BY_NOTHING) {
		pthread_mutex_lock( &(G.mx_sort) );
		    dump_sort();
		pthread_mutex_unlock( &(G.mx_sort) );
	    }
        }

        if( time( NULL ) - tt2 > 3 )
        {
            /* update the battery state */
            free(G.batt);
            G.batt = NULL;

            tt2 = time( NULL );
            G.batt = getBatteryString();

            /* update elapsed time */

            free(G.elapsed_time);
            G.elapsed_time=NULL;
            G.elapsed_time = getStringTimeFromSec(
            difftime(tt2, start_time) );


            /* flush the output files */

            if( G.f_cap != NULL ) fflush( G.f_cap );
            if( G.f_ivs != NULL ) fflush( G.f_ivs );
        }

        gettimeofday( &tv1, NULL );

        cycle_time = 1000000 * ( tv1.tv_sec  - tv3.tv_sec  )
                             + ( tv1.tv_usec - tv3.tv_usec );

        cycle_time2 = 1000000 * ( tv1.tv_sec  - tv4.tv_sec  )
                              + ( tv1.tv_usec - tv4.tv_usec );

        if( G.active_scan_sim > 0 && cycle_time2 > G.active_scan_sim*1000 )
        {
            gettimeofday( &tv4, NULL );
            send_probe_requests(wi, G.num_cards);
        }

        if( cycle_time > 500000 )
        {
            gettimeofday( &tv3, NULL );
            update_rx_quality( );
            if(G.s_iface != NULL)
            {
                check_monitor(wi, fd_raw, &fdh, G.num_cards);
                if(G.singlechan)
                    check_channel(wi, G.num_cards);
                if(G.singlefreq)
                    check_frequency(wi, G.num_cards);
            }
        }

        if(G.s_file != NULL)
        {
            /* Read one packet */
            n = sizeof( pkh );

            if( fread( &pkh, n, 1, G.f_cap_in ) != 1 )
            {
                memset(G.message, '\x00', sizeof(G.message));
                snprintf(G.message, sizeof(G.message), "][ Finished reading input file %s.\n", G.s_file);
                G.s_file = NULL;
                continue;
            }

            if( G.pfh_in.magic == TCPDUMP_CIGAM ) {
                SWAP32( pkh.caplen );
                SWAP32( pkh.len );
            }

            n = caplen = pkh.caplen;

            memset(buffer, 0, sizeof(buffer));
            h80211 = buffer;

            if( n <= 0 || n > (int) sizeof( buffer ) )
            {
                memset(G.message, '\x00', sizeof(G.message));
                snprintf(G.message, sizeof(G.message), "][ Finished reading input file %s.\n", G.s_file);
                G.s_file = NULL;
                continue;
            }

            if( fread( h80211, n, 1, G.f_cap_in ) != 1 )
            {
                memset(G.message, '\x00', sizeof(G.message));
                snprintf(G.message, sizeof(G.message), "][ Finished reading input file %s.\n", G.s_file);
                G.s_file = NULL;
                continue;
            }

            if( G.pfh_in.linktype == LINKTYPE_PRISM_HEADER )
            {
                if( h80211[7] == 0x40 )
                    n = 64;
                else
                    n = *(int *)( h80211 + 4 );

                if( n < 8 || n >= (int) caplen )
                    continue;

                memcpy( tmpbuf, h80211, caplen );
                caplen -= n;
                memcpy( h80211, tmpbuf + n, caplen );
            }

            if( G.pfh_in.linktype == LINKTYPE_RADIOTAP_HDR )
            {
                /* remove the radiotap header */

                n = *(unsigned short *)( h80211 + 2 );

                if( n <= 0 || n >= (int) caplen )
                    continue;

                memcpy( tmpbuf, h80211, caplen );
                caplen -= n;
                memcpy( h80211, tmpbuf + n, caplen );
            }

            if( G.pfh_in.linktype == LINKTYPE_PPI_HDR )
            {
                /* remove the PPI header */

                n = le16_to_cpu(*(unsigned short *)( h80211 + 2));

                if( n <= 0 || n>= (int) caplen )
                    continue;

                /* for a while Kismet logged broken PPI headers */
                if ( n == 24 && le16_to_cpu(*(unsigned short *)(h80211 + 8)) == 2 )
                    n = 32;

                if( n <= 0 || n>= (int) caplen )
                    continue;

                memcpy( tmpbuf, h80211, caplen );
                caplen -= n;
                memcpy( h80211, tmpbuf + n, caplen );
            }

            read_pkts++;

            if(read_pkts%10 == 0)
                usleep(1);
        }
        else if(G.s_iface != NULL)
        {
            /* capture one packet */

            FD_ZERO( &rfds );
            for(i=0; i<G.num_cards; i++)
            {
                FD_SET( fd_raw[i], &rfds );
            }

            tv0.tv_sec  = G.update_s;
            tv0.tv_usec = (G.update_s == 0) ? REFRESH_RATE : 0;

            gettimeofday( &tv1, NULL );

            if( select( fdh + 1, &rfds, NULL, NULL, &tv0 ) < 0 )
            {
                if( errno == EINTR )
                {
                    gettimeofday( &tv2, NULL );

                    time_slept += 1000000 * ( tv2.tv_sec  - tv1.tv_sec  )
                                        + ( tv2.tv_usec - tv1.tv_usec );

                    continue;
                }
                perror( "select failed" );

                /* Restore terminal */
                fprintf( stderr, "\33[?25h" );
                fflush( stdout );

                return( 1 );
            }
        }
        else
            usleep(1);

        gettimeofday( &tv2, NULL );

        time_slept += 1000000 * ( tv2.tv_sec  - tv1.tv_sec  )
                              + ( tv2.tv_usec - tv1.tv_usec );

        if( time_slept > REFRESH_RATE && time_slept > G.update_s * 1000000)
        {
            time_slept = 0;

            update_dataps();

            /* update the window size */

            if( ioctl( 0, TIOCGWINSZ, &(G.ws) ) < 0 )
            {
                G.ws.ws_row = 25;
                G.ws.ws_col = 80;
            }

            if( G.ws.ws_col <   1 ) G.ws.ws_col =   1;
            if( G.ws.ws_col > 300 ) G.ws.ws_col = 300;

            /* display the list of access points we have */

	    if(!G.do_pause) {
		pthread_mutex_lock( &(G.mx_print) );

		    fprintf( stderr, "\33[1;1H" );
		    dump_print( G.ws.ws_row, G.ws.ws_col, G.num_cards );
		    fprintf( stderr, "\33[J" );
		    fflush( stdout );

		pthread_mutex_unlock( &(G.mx_print) );
	    }
            continue;
        }

        if(G.s_file == NULL && G.s_iface != NULL)
        {
            fd_is_set = 0;

            for(i=0; i<G.num_cards; i++)
            {
                if( FD_ISSET( fd_raw[i], &rfds ) )
                {

                    memset(buffer, 0, sizeof(buffer));
                    h80211 = buffer;
                    if ((caplen = wi_read(wi[i], h80211, sizeof(buffer), &ri)) == -1) {
                        wi_read_failed++;
                        if(wi_read_failed > 1)
                        {
                            G.do_exit = 1;
                            break;
                        }
                        memset(G.message, '\x00', sizeof(G.message));
                        snprintf(G.message, sizeof(G.message), "][ interface %s down ", wi_get_ifname(wi[i]));

                        //reopen in monitor mode

                        strncpy(ifnam, wi_get_ifname(wi[i]), sizeof(ifnam)-1);
                        ifnam[sizeof(ifnam)-1] = 0;

                        wi_close(wi[i]);
                        wi[i] = wi_open(ifnam);
                        if (!wi[i]) {
                            printf("Can't reopen %s\n", ifnam);

                            /* Restore terminal */
                            fprintf( stderr, "\33[?25h" );
                            fflush( stdout );

                            exit(1);
                        }

                        fd_raw[i] = wi_fd(wi[i]);
                        if (fd_raw[i] > fdh)
                            fdh = fd_raw[i];

                        break;
//                         return 1;
                    }

                    read_pkts++;

                    wi_read_failed = 0;
                    dump_add_packet( h80211, caplen, &ri, i );
                }
            }
        }
        else if (G.s_file != NULL)
        {
            dump_add_packet( h80211, caplen, &ri, i );
        }
    }

    if(G.batt)
        free(G.batt);

    if(G.elapsed_time)
        free(G.elapsed_time);

    if(G.own_channels)
        free(G.own_channels);
    
    if(G.f_essid)
        free(G.f_essid);

    if(G.prefix)
        free(G.prefix);

    if(G.f_cap_name)
        free(G.f_cap_name);

    if(G.keyout)
        free(G.keyout);

#ifdef HAVE_PCRE
    if(G.f_essid_regex)
        pcre_free(G.f_essid_regex);
#endif

    for(i=0; i<G.num_cards; i++)
        wi_close(wi[i]);

    if (G.record_data) {
        if ( G. output_format_csv)  dump_write_csv();
        if ( G.output_format_kismet_csv) dump_write_kismet_csv();
        if ( G.output_format_kismet_netxml) dump_write_kismet_netxml();

        if ( G. output_format_csv || G.f_txt != NULL ) fclose( G.f_txt );
        if ( G.output_format_kismet_csv || G.f_kis != NULL ) fclose( G.f_kis );
        if ( G.output_format_kismet_netxml || G.f_kis_xml != NULL )
        {
			fclose( G.f_kis_xml );
			free(G.airodump_start_time);
		}
        if ( G.f_gps != NULL ) fclose( G.f_gps );
        if ( G.output_format_pcap ||  G.f_cap != NULL ) fclose( G.f_cap );
        if ( G.f_ivs != NULL ) fclose( G.f_ivs );
    }

    if( ! G.save_gps )
    {
        snprintf( (char *) buffer, 4096, "%s-%02d.gps", argv[2], G.f_index );
        unlink(  (char *) buffer );
    }

    ap_prv = NULL;
    ap_cur = G.ap_1st;

    while( ap_cur != NULL )
    {
		// Clean content of ap_cur list (first element: G.ap_1st)
        uniqueiv_wipe( ap_cur->uiv_root );

        list_tail_free(&(ap_cur->packets));

	if (G.manufList)
		free(ap_cur->manuf);

	if (G.detect_anomaly)
        	data_wipe(ap_cur->data_root);

        ap_prv = ap_cur;
        ap_cur = ap_cur->next;
    }

    ap_cur = G.ap_1st;

    while( ap_cur != NULL )
    {
		// Freeing AP List
        ap_next = ap_cur->next;

        if( ap_cur != NULL )
            free(ap_cur);

        ap_cur = ap_next;
    }

    st_cur = G.st_1st;
    st_next= NULL;

    while(st_cur != NULL)
    {
        st_next = st_cur->next;
	if (G.manufList)
		free(st_cur->manuf);
        free(st_cur);
        st_cur = st_next;
    }

    na_cur = G.na_1st;
    na_next= NULL;

    while(na_cur != NULL)
    {
        na_next = na_cur->next;
        free(na_cur);
        na_cur = na_next;
    }

    if (G.manufList) {
        oui_cur = G.manufList;
        while (oui_cur != NULL) {
            oui_next = oui_cur->next;
	    free(oui_cur);
	    oui_cur = oui_next;
        }
    }

    fprintf( stderr, "\33[?25h" );
    fflush( stdout );

    return( 0 );
}