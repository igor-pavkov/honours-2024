int avi_update_header(avi_t *AVI)
{
	int njunk, ms_per_frame, frate, flag;
	int movi_len, hdrl_start, strl_start;
	u32 j;
	unsigned char AVI_header[HEADERBYTES];
	u32 nhb;
	unsigned int xd_size, xd_size_align2;

	//assume max size
	movi_len = AVI_MAX_LEN - HEADERBYTES + 4;

	//assume index will be written
//	int hasIndex=1;

	if(AVI->fps < 0.001) {
		frate=0;
		ms_per_frame=0;
	} else {
		frate = (int) (FRAME_RATE_SCALE*AVI->fps + 0.5);
		ms_per_frame=(int) (1000000/AVI->fps + 0.5);
	}

	/* Prepare the file header */

	nhb = 0;

	/* The RIFF header */

	OUT4CC ("RIFF");
	OUTLONG(movi_len);    // assume max size
	OUT4CC ("AVI ");

	/* Start the header list */

	OUT4CC ("LIST");
	OUTLONG(0);        /* Length of list in bytes, don't know yet */
	hdrl_start = nhb;  /* Store start position */
	OUT4CC ("hdrl");

	/* The main AVI header */

	/* The Flags in AVI File header */

#define AVIF_HASINDEX           0x00000010      /* Index at end of file */
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800      /* Use CKType to find key frames */
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000

	OUT4CC ("avih");
	OUTLONG(56);                 /* # of bytes to follow */
	OUTLONG(ms_per_frame);       /* Microseconds per frame */
	//ThOe ->0
	//   OUTLONG(10000000);           /* MaxBytesPerSec, I hope this will never be used */
	OUTLONG(0);
	OUTLONG(0);                  /* PaddingGranularity (whatever that might be) */
	/* Other sources call it 'reserved' */
	flag = AVIF_ISINTERLEAVED;
	//if (hasIndex)
		flag |= AVIF_HASINDEX;
	if (/*hasIndex && */AVI->must_use_index)
		flag |= AVIF_MUSTUSEINDEX;
	OUTLONG(flag);               /* Flags */
	OUTLONG(0);                  // no frames yet
	OUTLONG(0);                  /* InitialFrames */

	OUTLONG(AVI->anum+1);

	OUTLONG(0);                  /* SuggestedBufferSize */
	OUTLONG(AVI->width);         /* Width */
	OUTLONG(AVI->height);        /* Height */
	/* MS calls the following 'reserved': */
	OUTLONG(0);                  /* TimeScale:  Unit used to measure time */
	OUTLONG(0);                  /* DataRate:   Data rate of playback     */
	OUTLONG(0);                  /* StartTime:  Starting time of AVI data */
	OUTLONG(0);                  /* DataLength: Size of AVI data chunk    */


	/* Start the video stream list ---------------------------------- */

	OUT4CC ("LIST");
	OUTLONG(0);        /* Length of list in bytes, don't know yet */
	strl_start = nhb;  /* Store start position */
	OUT4CC ("strl");

	/* The video stream header */

	OUT4CC ("strh");
	OUTLONG(56);                 /* # of bytes to follow */
	OUT4CC ("vids");             /* Type */
	OUT4CC (AVI->compressor);    /* Handler */
	OUTLONG(0);                  /* Flags */
	OUTLONG(0);                  /* Reserved, MS says: wPriority, wLanguage */
	OUTLONG(0);                  /* InitialFrames */
	OUTLONG(FRAME_RATE_SCALE);              /* Scale */
	OUTLONG(frate);              /* Rate: Rate/Scale == samples/second */
	OUTLONG(0);                  /* Start */
	OUTLONG(0);                  // no frames yet
	OUTLONG(0);                  /* SuggestedBufferSize */
	OUTLONG(-1);                 /* Quality */
	OUTLONG(0);                  /* SampleSize */
	OUTLONG(0);                  /* Frame */
	OUTLONG(0);                  /* Frame */
	//   OUTLONG(0);                  /* Frame */
	//OUTLONG(0);                  /* Frame */

	/* The video stream format */

	xd_size        = AVI->extradata_size;
	xd_size_align2 = (AVI->extradata_size+1) & ~1;

	OUT4CC ("strf");
	OUTLONG(40 + xd_size_align2);/* # of bytes to follow */
	OUTLONG(40 + xd_size);	/* Size */
	OUTLONG(AVI->width);         /* Width */
	OUTLONG(AVI->height);        /* Height */
	OUTSHRT(1);
	OUTSHRT(24);     /* Planes, Count */
	OUT4CC (AVI->compressor);    /* Compression */
	// ThOe (*3)
	OUTLONG(AVI->width*AVI->height*3);  /* SizeImage (in bytes?) */
	OUTLONG(0);                  /* XPelsPerMeter */
	OUTLONG(0);                  /* YPelsPerMeter */
	OUTLONG(0);                  /* ClrUsed: Number of colors used */
	OUTLONG(0);                  /* ClrImportant: Number of colors important */

	// write extradata
	if (xd_size > 0 && AVI->extradata) {
		OUTMEM(AVI->extradata, xd_size);
		if (xd_size != xd_size_align2) {
			OUTCHR(0);
		}
	}

	/* Finish stream list, i.e. put number of bytes in the list to proper pos */

	long2str(AVI_header+strl_start-4,nhb-strl_start);


	/* Start the audio stream list ---------------------------------- */

	for(j=0; j<AVI->anum; ++j) {
		int sampsize = avi_sampsize(AVI, j);

		OUT4CC ("LIST");
		OUTLONG(0);        /* Length of list in bytes, don't know yet */
		strl_start = nhb;  /* Store start position */
		OUT4CC ("strl");

		/* The audio stream header */

		OUT4CC ("strh");
		OUTLONG(56);            /* # of bytes to follow */
		OUT4CC ("auds");

		// -----------
		// ThOe
		OUTLONG(0);             /* Format (Optionally) */
		// -----------

		OUTLONG(0);             /* Flags */
		OUTLONG(0);             /* Reserved, MS says: wPriority, wLanguage */
		OUTLONG(0);             /* InitialFrames */

		// ThOe /4
		OUTLONG(sampsize/4);      /* Scale */
		OUTLONG(1000*AVI->track[j].mp3rate/8);
		OUTLONG(0);             /* Start */
		OUTLONG(4*AVI->track[j].audio_bytes/sampsize);   /* Length */
		OUTLONG(0);             /* SuggestedBufferSize */
		OUTLONG(-1);            /* Quality */

		// ThOe /4
		OUTLONG(sampsize/4);    /* SampleSize */

		OUTLONG(0);             /* Frame */
		OUTLONG(0);             /* Frame */
		//       OUTLONG(0);             /* Frame */
		//OUTLONG(0);             /* Frame */

		/* The audio stream format */

		OUT4CC ("strf");
		OUTLONG(16);                   /* # of bytes to follow */
		OUTSHRT(AVI->track[j].a_fmt);           /* Format */
		OUTSHRT(AVI->track[j].a_chans);         /* Number of channels */
		OUTLONG(AVI->track[j].a_rate);          /* SamplesPerSec */
		// ThOe
		OUTLONG(1000*AVI->track[j].mp3rate/8);
		//ThOe (/4)

		OUTSHRT(sampsize/4);           /* BlockAlign */


		OUTSHRT(AVI->track[j].a_bits);          /* BitsPerSample */

		/* Finish stream list, i.e. put number of bytes in the list to proper pos */

		long2str(AVI_header+strl_start-4,nhb-strl_start);
	}

	/* Finish header list */

	long2str(AVI_header+hdrl_start-4,nhb-hdrl_start);


	/* Calculate the needed amount of junk bytes, output junk */

	njunk = HEADERBYTES - nhb - 8 - 12;

	/* Safety first: if njunk <= 0, somebody has played with
	   HEADERBYTES without knowing what (s)he did.
	   This is a fatal error */

	if(njunk<=0)
	{
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[avilib] AVI_close_output_file: # of header bytes too small\n"));
		exit(1);
	}

	OUT4CC ("JUNK");
	OUTLONG(njunk);
	memset(AVI_header+nhb,0,njunk);

	nhb += njunk;

	/* Start the movi list */

	OUT4CC ("LIST");
	OUTLONG(movi_len); /* Length of list in bytes */
	OUT4CC ("movi");

	/* Output the header, truncate the file to the number of bytes
	   actually written, report an error if someting goes wrong */

	if ( (gf_fseek(AVI->fdes, 0, SEEK_SET) ==(u64)-1) ||
	        avi_write(AVI->fdes,(char *)AVI_header,HEADERBYTES)!=HEADERBYTES ||
	        (gf_fseek(AVI->fdes,AVI->pos,SEEK_SET)==(u64)-1)
	   ) {
		AVI_errno = AVI_ERR_CLOSE;
		return -1;
	}

	return 0;
}