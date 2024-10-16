static int avi_close_output_file(avi_t *AVI)
{
	int njunk, hasIndex, ms_per_frame, frate, idxerror, flag;
	u64 movi_len;
	int hdrl_start, strl_start;
	u32 j;
	unsigned char AVI_header[HEADERBYTES];
	int nhb;
	unsigned int xd_size, xd_size_align2;

#ifdef INFO_LIST
	int info_len;
	int id_len, real_id_len;
	int info_start_pos;
//   time_t calptr;
#endif

	/* Calculate length of movi list */

	// dump the rest of the index
	if (AVI->is_opendml) {
		int cur_std_idx = AVI->video_superindex->nEntriesInUse-1;
		u32 audtr;

#ifdef DEBUG_ODML
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] ODML dump the rest indices\n"));
#endif
		avi_ixnn_entry (AVI, AVI->video_superindex->stdindex[cur_std_idx],
		                &AVI->video_superindex->aIndex[cur_std_idx]);

		AVI->video_superindex->aIndex[cur_std_idx].dwDuration =
		    AVI->video_superindex->stdindex[cur_std_idx]->nEntriesInUse - 1;

		for (audtr = 0; audtr < AVI->anum; audtr++) {
			if (!AVI->track[audtr].audio_superindex) {
				// not initialized -> no index
				continue;
			}
			avi_ixnn_entry (AVI, AVI->track[audtr].audio_superindex->stdindex[cur_std_idx],
			                &AVI->track[audtr].audio_superindex->aIndex[cur_std_idx]);
			AVI->track[audtr].audio_superindex->aIndex[cur_std_idx].dwDuration =
			    AVI->track[audtr].audio_superindex->stdindex[cur_std_idx]->nEntriesInUse - 1;
			if (AVI->track[audtr].a_fmt == 0x1) {
				AVI->track[audtr].audio_superindex->aIndex[cur_std_idx].dwDuration *=
				    AVI->track[audtr].a_bits*AVI->track[audtr].a_rate*AVI->track[audtr].a_chans/800;
			}
		}
		// The AVI->video_superindex->nEntriesInUse contains the offset
		AVI->video_superindex->stdindex[ cur_std_idx+1 ]->qwBaseOffset = AVI->pos;

		// Correct!
		movi_len = AVI->video_superindex->stdindex[ 1 ]->qwBaseOffset - HEADERBYTES+4 - AVI->n_idx*16 - 8;
	} else {
		movi_len = AVI->pos - HEADERBYTES + 4;
	}


	/* Try to output the index entries. This may fail e.g. if no space
	   is left on device. We will report this as an error, but we still
	   try to write the header correctly (so that the file still may be
	   readable in the most cases */

	idxerror = 0;
	hasIndex = 1;
	if (!AVI->is_opendml) {
		//   GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] pos=%lu, index_len=%ld             \n", AVI->pos, AVI->n_idx*16));
		int ret = avi_add_chunk(AVI, (unsigned char *)"idx1", (unsigned char *)AVI->idx, AVI->n_idx*16);
		hasIndex = (ret==0);
		//GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] pos=%lu, index_len=%d\n", AVI->pos, hasIndex));

		if(ret) {
			idxerror = 1;
			AVI_errno = AVI_ERR_WRITE_INDEX;
		}
	}

	/* Calculate Microseconds per frame */

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
	if (AVI->is_opendml) {
		OUTLONG(AVI->video_superindex->stdindex[ 1 ]->qwBaseOffset - 8);    /* # of bytes to follow */
	} else {
		OUTLONG(AVI->pos - 8);    /* # of bytes to follow */
	}

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
	if(hasIndex) flag |= AVIF_HASINDEX;
	if(hasIndex && AVI->must_use_index) flag |= AVIF_MUSTUSEINDEX;
	OUTLONG(flag);               /* Flags */
	OUTLONG(AVI->video_frames);  /* TotalFrames */
	OUTLONG(0);                  /* InitialFrames */

	OUTLONG(AVI->anum+1);
//   if (AVI->track[0].audio_bytes)
//      { OUTLONG(2); }           /* Streams */
//   else
//      { OUTLONG(1); }           /* Streams */

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
	OUTLONG(FRAME_RATE_SCALE);   /* Scale */
	OUTLONG(frate);              /* Rate: Rate/Scale == samples/second */
	OUTLONG(0);                  /* Start */
	OUTLONG(AVI->video_frames);  /* Length */
	OUTLONG(AVI->max_len);       /* SuggestedBufferSize */
	OUTLONG(0);                  /* Quality */
	OUTLONG(0);                  /* SampleSize */
	OUTLONG(0);                  /* Frame */
	OUTLONG(0);                  /* Frame */
	//OUTLONG(0);                  /* Frame */
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

	// write extradata if present
	if (xd_size > 0 && AVI->extradata) {
		OUTMEM(AVI->extradata, xd_size);
		if (xd_size != xd_size_align2) {
			OUTCHR(0);
		}
	}

	// dump index of indices for audio
	if (AVI->is_opendml) {
		u32 k;

		OUT4CC(AVI->video_superindex->fcc);
		OUTLONG(2+1+1+4+4+3*4 + AVI->video_superindex->nEntriesInUse * (8+4+4));
		OUTSHRT(AVI->video_superindex->wLongsPerEntry);
		OUTCHR(AVI->video_superindex->bIndexSubType);
		OUTCHR(AVI->video_superindex->bIndexType);
		OUTLONG(AVI->video_superindex->nEntriesInUse);
		OUT4CC(AVI->video_superindex->dwChunkId);
		OUTLONG(0);
		OUTLONG(0);
		OUTLONG(0);


		for (k = 0; k < AVI->video_superindex->nEntriesInUse; k++) {
			u32 r = (u32) ((AVI->video_superindex->aIndex[k].qwOffset >> 32) & 0xffffffff);
			u32 s = (u32) ((AVI->video_superindex->aIndex[k].qwOffset) & 0xffffffff);

			OUTLONG(s);
			OUTLONG(r);
			OUTLONG(AVI->video_superindex->aIndex[k].dwSize);
			OUTLONG(AVI->video_superindex->aIndex[k].dwDuration);
		}

	}

	/* Finish stream list, i.e. put number of bytes in the list to proper pos */

	long2str(AVI_header+strl_start-4,nhb-strl_start);

	/* Start the audio stream list ---------------------------------- */

	for(j=0; j<AVI->anum; ++j) {

		//if (AVI->track[j].a_chans && AVI->track[j].audio_bytes)
		{
			unsigned int nBlockAlign = 0;
			unsigned int avgbsec = 0;
			unsigned int scalerate = 0;

			int sampsize = avi_sampsize(AVI, j);
			sampsize = AVI->track[j].a_fmt==0x1?sampsize*4:sampsize;

			nBlockAlign = (AVI->track[j].a_rate<32000)?576:1152;
			/*
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] XXX sampsize (%d) block (%ld) rate (%ld) audio_bytes (%ld) mp3rate(%ld,%ld)\n",
			 sampsize, nBlockAlign, AVI->track[j].a_rate,
			 (int int)AVI->track[j].audio_bytes,
			 1000*AVI->track[j].mp3rate/8, AVI->track[j].mp3rate));
			 */

			if (AVI->track[j].a_fmt==0x1) {
				sampsize = (AVI->track[j].a_chans<2)?sampsize/2:sampsize;
				avgbsec = AVI->track[j].a_rate*sampsize/4;
				scalerate = AVI->track[j].a_rate*sampsize/4;
			} else  {
				avgbsec = 1000*AVI->track[j].mp3rate/8;
				scalerate = 1000*AVI->track[j].mp3rate/8;
			}

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

			// VBR
			if (AVI->track[j].a_fmt == 0x55 && AVI->track[j].a_vbr) {
				OUTLONG(nBlockAlign);                   /* Scale */
				OUTLONG(AVI->track[j].a_rate);          /* Rate */
				OUTLONG(0);                             /* Start */
				OUTLONG(AVI->track[j].audio_chunks);    /* Length */
				OUTLONG(0);                      /* SuggestedBufferSize */
				OUTLONG(0);                             /* Quality */
				OUTLONG(0);                             /* SampleSize */
				OUTLONG(0);                             /* Frame */
				OUTLONG(0);                             /* Frame */
			} else {
				OUTLONG(sampsize/4);                    /* Scale */
				OUTLONG(scalerate);  /* Rate */
				OUTLONG(0);                             /* Start */
				OUTLONG(4*AVI->track[j].audio_bytes/sampsize);   /* Length */
				OUTLONG(0);                             /* SuggestedBufferSize */
				OUTLONG(0xffffffff);                             /* Quality */
				OUTLONG(sampsize/4);                    /* SampleSize */
				OUTLONG(0);                             /* Frame */
				OUTLONG(0);                             /* Frame */
			}

			/* The audio stream format */

			OUT4CC ("strf");

			if (AVI->track[j].a_fmt == 0x55 && AVI->track[j].a_vbr) {

				OUTLONG(30);                            /* # of bytes to follow */ // mplayer writes 28
				OUTSHRT(AVI->track[j].a_fmt);           /* Format */                  // 2
				OUTSHRT(AVI->track[j].a_chans);         /* Number of channels */      // 2
				OUTLONG(AVI->track[j].a_rate);          /* SamplesPerSec */           // 4
				//ThOe/tibit
				OUTLONG(1000*AVI->track[j].mp3rate/8);  /* maybe we should write an avg. */ // 4
				OUTSHRT(nBlockAlign);                   /* BlockAlign */              // 2
				OUTSHRT(AVI->track[j].a_bits);          /* BitsPerSample */           // 2

				OUTSHRT(12);                           /* cbSize */                   // 2
				OUTSHRT(1);                            /* wID */                      // 2
				OUTLONG(2);                            /* fdwFlags */                 // 4
				OUTSHRT(nBlockAlign);                  /* nBlockSize */               // 2
				OUTSHRT(1);                            /* nFramesPerBlock */          // 2
				OUTSHRT(0);                            /* nCodecDelay */              // 2

			} else if (AVI->track[j].a_fmt == 0x55 && !AVI->track[j].a_vbr) {

				OUTLONG(30);                            /* # of bytes to follow */
				OUTSHRT(AVI->track[j].a_fmt);           /* Format */
				OUTSHRT(AVI->track[j].a_chans);         /* Number of channels */
				OUTLONG(AVI->track[j].a_rate);          /* SamplesPerSec */
				//ThOe/tibit
				OUTLONG(1000*AVI->track[j].mp3rate/8);
				OUTSHRT(sampsize/4);                    /* BlockAlign */
				OUTSHRT(AVI->track[j].a_bits);          /* BitsPerSample */

				OUTSHRT(12);                           /* cbSize */
				OUTSHRT(1);                            /* wID */
				OUTLONG(2);                            /* fdwFlags */
				OUTSHRT(nBlockAlign);                  /* nBlockSize */
				OUTSHRT(1);                            /* nFramesPerBlock */
				OUTSHRT(0);                            /* nCodecDelay */

			} else {

				OUTLONG(18);                   /* # of bytes to follow */
				OUTSHRT(AVI->track[j].a_fmt);           /* Format */
				OUTSHRT(AVI->track[j].a_chans);         /* Number of channels */
				OUTLONG(AVI->track[j].a_rate);          /* SamplesPerSec */
				//ThOe/tibit
				OUTLONG(avgbsec);  /* Avg bytes/sec */
				OUTSHRT(sampsize/4);                    /* BlockAlign */
				OUTSHRT(AVI->track[j].a_bits);          /* BitsPerSample */
				OUTSHRT(0);                           /* cbSize */

			}
		}
		if (AVI->is_opendml) {
			u32 k;

			if (!AVI->track[j].audio_superindex) {
				// not initialized -> no index
				continue;
			}

			OUT4CC(AVI->track[j].audio_superindex->fcc);    /* "indx" */
			OUTLONG(2+1+1+4+4+3*4 + AVI->track[j].audio_superindex->nEntriesInUse * (8+4+4));
			OUTSHRT(AVI->track[j].audio_superindex->wLongsPerEntry);
			OUTCHR(AVI->track[j].audio_superindex->bIndexSubType);
			OUTCHR(AVI->track[j].audio_superindex->bIndexType);
			OUTLONG(AVI->track[j].audio_superindex->nEntriesInUse);
			OUT4CC(AVI->track[j].audio_superindex->dwChunkId);
			OUTLONG(0);
			OUTLONG(0);
			OUTLONG(0);

			for (k = 0; k < AVI->track[j].audio_superindex->nEntriesInUse; k++) {
				u32 r = (u32) ((AVI->track[j].audio_superindex->aIndex[k].qwOffset >> 32) & 0xffffffff);
				u32 s = (u32) ((AVI->track[j].audio_superindex->aIndex[k].qwOffset) & 0xffffffff);

				/*
				GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] AUD[%d] NrEntries %d/%ld (%c%c%c%c) |0x%llX|%ld|%ld| \n",  j, k,
				        AVI->track[j].audio_superindex->nEntriesInUse,
				    AVI->track[j].audio_superindex->dwChunkId[0], AVI->track[j].audio_superindex->dwChunkId[1],
				    AVI->track[j].audio_superindex->dwChunkId[2], AVI->track[j].audio_superindex->dwChunkId[3],
				    AVI->track[j].audio_superindex->aIndex[k].qwOffset,
				    AVI->track[j].audio_superindex->aIndex[k].dwSize,
				    AVI->track[j].audio_superindex->aIndex[k].dwDuration
				  ));
				  */

				OUTLONG(s);
				OUTLONG(r);
				OUTLONG(AVI->track[j].audio_superindex->aIndex[k].dwSize);
				OUTLONG(AVI->track[j].audio_superindex->aIndex[k].dwDuration);
			}
		}
		/* Finish stream list, i.e. put number of bytes in the list to proper pos */
		long2str(AVI_header+strl_start-4,nhb-strl_start);
	}

	if (AVI->is_opendml) {
		OUT4CC("LIST");
		OUTLONG(16);
		OUT4CC("odml");
		OUT4CC("dmlh");
		OUTLONG(4);
		OUTLONG(AVI->total_frames);
	}

	/* Finish header list */

	long2str(AVI_header+hdrl_start-4,nhb-hdrl_start);


	// add INFO list --- (0.6.0pre4)

#ifdef INFO_LIST
	OUT4CC ("LIST");

	info_start_pos = nhb;
	info_len = MAX_INFO_STRLEN + 12;
	OUTLONG(info_len); // rewritten later
	OUT4CC ("INFO");

	OUT4CC ("ISFT");
	//OUTLONG(MAX_INFO_STRLEN);
	memset(id_str, 0, MAX_INFO_STRLEN);
	if (gf_sys_is_test_mode()) {
		snprintf(id_str, MAX_INFO_STRLEN, "GPAC/avilib");
	} else {
		snprintf(id_str, MAX_INFO_STRLEN, "GPAC/avilib-%s", gf_gpac_version());
	}
	real_id_len = id_len = (u32) strlen(id_str)+1;
	if (id_len&1) id_len++;

	OUTLONG(real_id_len);

	memset(AVI_header+nhb, 0, id_len);
	memcpy(AVI_header+nhb, id_str, id_len);
	nhb += id_len;

	info_len = 0;

	// write correct len
	long2str(AVI_header+info_start_pos, info_len + id_len + 4+4+4);

	nhb += info_len;

//   OUT4CC ("ICMT");
//   OUTLONG(MAX_INFO_STRLEN);

//   calptr=time(NULL);
//   sprintf(id_str, "\t%s %s", ctime(&calptr), "");
//   memset(AVI_header+nhb, 0, MAX_INFO_STRLEN);
//   memcpy(AVI_header+nhb, id_str, 25);
//   nhb += MAX_INFO_STRLEN;
#endif

	// ----------------------------

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

	if ( (gf_fseek(AVI->fdes,0,SEEK_SET)==(u64)-1) ||
	        avi_write(AVI->fdes,(char *)AVI_header,HEADERBYTES)!=HEADERBYTES
//		|| ftruncate(AVI->fdes,AVI->pos)<0
	   )
	{
		AVI_errno = AVI_ERR_CLOSE;
		return -1;
	}


	// Fix up the empty additional RIFF and LIST chunks
	if (AVI->is_opendml) {
		u32 k;
		char f[4];
		u32 len;

		for (k=1; k<AVI->video_superindex->nEntriesInUse; k++) {
			// the len of the RIFF Chunk
			gf_fseek(AVI->fdes, AVI->video_superindex->stdindex[k]->qwBaseOffset+4, SEEK_SET);
			len = (u32) (AVI->video_superindex->stdindex[k+1]->qwBaseOffset - AVI->video_superindex->stdindex[k]->qwBaseOffset - 8);
			long2str((unsigned char *)f, len);
			avi_write(AVI->fdes, f, 4);

			// len of the LIST/movi chunk
			gf_fseek(AVI->fdes, 8, SEEK_CUR);
			len -= 12;
			long2str((unsigned char *)f, len);
			avi_write(AVI->fdes, f, 4);
		}
	}


	if(idxerror) return -1;

	return 0;
}