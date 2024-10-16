int dump_initialize( char *prefix, int ivs_only )
{
    int i, ofn_len;
    FILE *f;
    char * ofn = NULL;


    /* If you only want to see what happening, send all data to /dev/null */

    if ( prefix == NULL || strlen( prefix ) == 0) {
	    return( 0 );
    }

	/* Create a buffer of the length of the prefix + '-' + 2 numbers + '.'
	   + longest extension ("kismet.netxml") + terminating 0. */
	ofn_len = strlen(prefix) + 1 + 2 + 1 + 13 + 1;
	ofn = (char *)calloc(1, ofn_len);

    G.f_index = 1;


	/* Make sure no file with the same name & all possible file extensions. */
    do
    {
        for( i = 0; i < NB_EXTENSIONS; i++ )
        {
			memset(ofn, 0, ofn_len);
            snprintf( ofn,  ofn_len, "%s-%02d.%s",
                      prefix, G.f_index, f_ext[i] );

            if( ( f = fopen( ofn, "rb+" ) ) != NULL )
            {
                fclose( f );
                G.f_index++;
                break;
            }
        }
    }
    /* If we did all extensions then no file with that name or extension exist
       so we can use that number */
    while( i < NB_EXTENSIONS );

    G.prefix = (char *) malloc(strlen(prefix) + 1);
    memcpy(G.prefix, prefix, strlen(prefix) + 1);

    /* create the output CSV file */

	if (G.output_format_csv) {
		memset(ofn, 0, ofn_len);
		snprintf( ofn,  ofn_len, "%s-%02d.%s",
				  prefix, G.f_index, AIRODUMP_NG_CSV_EXT );

		if( ( G.f_txt = fopen( ofn, "wb+" ) ) == NULL )
		{
			perror( "fopen failed" );
			fprintf( stderr, "Could not create \"%s\".\n", ofn );
			free( ofn );
			return( 1 );
		}
	}

    /* create the output Kismet CSV file */
	if (G.output_format_kismet_csv) {
		memset(ofn, 0, ofn_len);
		snprintf( ofn,  ofn_len, "%s-%02d.%s",
				  prefix, G.f_index, KISMET_CSV_EXT );

		if( ( G.f_kis = fopen( ofn, "wb+" ) ) == NULL )
		{
			perror( "fopen failed" );
			fprintf( stderr, "Could not create \"%s\".\n", ofn );
			free( ofn );
			return( 1 );
		}
	}

	/* create the output GPS file */

    if (G.usegpsd)
    {
        memset(ofn, 0, ofn_len);
        snprintf( ofn,  ofn_len, "%s-%02d.%s",
                  prefix, G.f_index, AIRODUMP_NG_GPS_EXT );

        if( ( G.f_gps = fopen( ofn, "wb+" ) ) == NULL )
        {
            perror( "fopen failed" );
            fprintf( stderr, "Could not create \"%s\".\n", ofn );
            free( ofn );
            return( 1 );
        }
    }

    /* Create the output kismet.netxml file */

	if (G.output_format_kismet_netxml) {
		memset(ofn, 0, ofn_len);
		snprintf( ofn,  ofn_len, "%s-%02d.%s",
				  prefix, G.f_index, KISMET_NETXML_EXT );

		if( ( G.f_kis_xml = fopen( ofn, "wb+" ) ) == NULL )
		{
			perror( "fopen failed" );
			fprintf( stderr, "Could not create \"%s\".\n", ofn );
			free( ofn );
			return( 1 );
		}
	}

    /* create the output packet capture file */
    if( G.output_format_pcap )
    {
        struct pcap_file_header pfh;

        memset(ofn, 0, ofn_len);
        snprintf( ofn,  ofn_len, "%s-%02d.%s",
                  prefix, G.f_index, AIRODUMP_NG_CAP_EXT );

        if( ( G.f_cap = fopen( ofn, "wb+" ) ) == NULL )
        {
            perror( "fopen failed" );
            fprintf( stderr, "Could not create \"%s\".\n", ofn );
            free( ofn );
            return( 1 );
        }

        G.f_cap_name = (char *) malloc( strlen( ofn ) + 1 );
        memcpy( G.f_cap_name, ofn, strlen( ofn ) + 1 );
        free( ofn );

        pfh.magic           = TCPDUMP_MAGIC;
        pfh.version_major   = PCAP_VERSION_MAJOR;
        pfh.version_minor   = PCAP_VERSION_MINOR;
        pfh.thiszone        = 0;
        pfh.sigfigs         = 0;
        pfh.snaplen         = 65535;
        pfh.linktype        = LINKTYPE_IEEE802_11;

        if( fwrite( &pfh, 1, sizeof( pfh ), G.f_cap ) !=
                    (size_t) sizeof( pfh ) )
        {
            perror( "fwrite(pcap file header) failed" );
            return( 1 );
        }
    } else if ( ivs_only ) {
        struct ivs2_filehdr fivs2;

        fivs2.version = IVS2_VERSION;

        memset(ofn, 0, ofn_len);
        snprintf( ofn,  ofn_len, "%s-%02d.%s",
                  prefix, G.f_index, IVS2_EXTENSION );

        if( ( G.f_ivs = fopen( ofn, "wb+" ) ) == NULL )
        {
            perror( "fopen failed" );
            fprintf( stderr, "Could not create \"%s\".\n", ofn );
            free( ofn );
            return( 1 );
        }
        free( ofn );

        if( fwrite( IVS2_MAGIC, 1, 4, G.f_ivs ) != (size_t) 4 )
        {
            perror( "fwrite(IVs file MAGIC) failed" );
            return( 1 );
        }

        if( fwrite( &fivs2, 1, sizeof(struct ivs2_filehdr), G.f_ivs ) != (size_t) sizeof(struct ivs2_filehdr) )
        {
            perror( "fwrite(IVs file header) failed" );
            return( 1 );
        }
    }

    return( 0 );
}