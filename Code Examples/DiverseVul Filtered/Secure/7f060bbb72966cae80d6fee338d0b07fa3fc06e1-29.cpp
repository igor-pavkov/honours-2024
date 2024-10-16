static int avi_add_odml_index_entry(avi_t *AVI, unsigned char *tag, int flags, u64 pos, unsigned int len)
{
	char fcc[5];

	int audio = (strchr ((char*)tag, 'w')?1:0);
	int video = !audio;

	unsigned int cur_std_idx;
	u32 audtr;
	s64 towrite = 0;

	if (video) {

		if (!AVI->video_superindex) {
			if (avi_init_super_index(AVI, (unsigned char *)"ix00", &AVI->video_superindex) < 0) return -1;
			AVI->video_superindex->nEntriesInUse++;
			cur_std_idx = AVI->video_superindex->nEntriesInUse-1;

			if (avi_add_std_index (AVI, (unsigned char *)"ix00", (unsigned char *)"00db", AVI->video_superindex->stdindex[ cur_std_idx ]) < 0)
				return -1;
		} // init

	} // video

	if (audio) {

		fcc[0] = 'i';
		fcc[1] = 'x';
		fcc[2] = tag[0];
		fcc[3] = tag[1];
		fcc[4] = '\0';
		if (!AVI->track[AVI->aptr].audio_superindex) {

#ifdef DEBUG_ODML
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] ODML: fcc = %s\n", fcc));
#endif
			if (avi_init_super_index(AVI, (unsigned char *)fcc, &AVI->track[AVI->aptr].audio_superindex) < 0) return -1;


			AVI->track[AVI->aptr].audio_superindex->nEntriesInUse++;

			sprintf(fcc, "ix%02d", AVI->aptr+1);
			if (avi_add_std_index (AVI, (unsigned char *)fcc, tag, AVI->track[AVI->aptr].audio_superindex->stdindex[
			                           AVI->track[AVI->aptr].audio_superindex->nEntriesInUse - 1 ]) < 0
			   ) return -1;
		} // init

	}

	towrite = 0;
	if (AVI->video_superindex) {

		cur_std_idx = AVI->video_superindex->nEntriesInUse-1;
		towrite += AVI->video_superindex->stdindex[cur_std_idx]->nEntriesInUse*8
		           + 4+4+2+1+1+4+4+8+4;
		if (cur_std_idx == 0) {
			towrite += AVI->n_idx*16 + 8;
			towrite += HEADERBYTES;
		}
	}

	for (audtr=0; audtr<AVI->anum; audtr++) {
		if (AVI->track[audtr].audio_superindex) {
			cur_std_idx = AVI->track[audtr].audio_superindex->nEntriesInUse-1;
			towrite += AVI->track[audtr].audio_superindex->stdindex[cur_std_idx]->nEntriesInUse*8
			           + 4+4+2+1+1+4+4+8+4;
		}
	}
	towrite += len + (len&1) + 8;

	//GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] ODML: towrite = 0x%llX = %"LLD"\n", towrite, towrite));

	if (AVI->video_superindex &&
	        (s64)(AVI->pos+towrite) > (s64)((s64) AVI->new_riff_threshold*AVI->video_superindex->nEntriesInUse)) {

		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] Adding a new RIFF chunk: %d\n", AVI->video_superindex->nEntriesInUse));

		// rotate ALL indices
		AVI->video_superindex->nEntriesInUse++;
		cur_std_idx = AVI->video_superindex->nEntriesInUse-1;

		if (AVI->video_superindex->nEntriesInUse > NR_IXNN_CHUNKS) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[avilib] Internal error in avilib - redefine NR_IXNN_CHUNKS\n"));
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] cur_std_idx=%d NR_IXNN_CHUNKS=%d"
			                                        "POS=%"LLD" towrite=%"LLD"\n",
			                                        cur_std_idx,NR_IXNN_CHUNKS, AVI->pos, towrite));
			return -1;
		}

		if (avi_add_std_index (AVI, (unsigned char *)"ix00", (unsigned char *)"00db", AVI->video_superindex->stdindex[ cur_std_idx ]) < 0)
			return -1;

		for (audtr = 0; audtr < AVI->anum; audtr++) {
			char aud[5];
			if (!AVI->track[audtr].audio_superindex) {
				// not initialized -> no index
				continue;
			}
			AVI->track[audtr].audio_superindex->nEntriesInUse++;

			sprintf(fcc, "ix%02d", audtr+1);
			sprintf(aud, "0%01dwb", audtr+1);
			if (avi_add_std_index (AVI, (unsigned char *)fcc, (unsigned char *)aud, AVI->track[audtr].audio_superindex->stdindex[
			                           AVI->track[audtr].audio_superindex->nEntriesInUse - 1 ]) < 0
			   ) return -1;
		}

		// write the new riff;
		if (cur_std_idx > 0) {

			// dump the _previous_ == already finished index
			avi_ixnn_entry (AVI, AVI->video_superindex->stdindex[cur_std_idx - 1],
			                &AVI->video_superindex->aIndex[cur_std_idx - 1]);
			AVI->video_superindex->aIndex[cur_std_idx - 1].dwDuration =
			    AVI->video_superindex->stdindex[cur_std_idx - 1]->nEntriesInUse - 1;

			for (audtr = 0; audtr < AVI->anum; audtr++) {

				if (!AVI->track[audtr].audio_superindex) {
					// not initialized -> no index
					continue;
				}
				avi_ixnn_entry (AVI, AVI->track[audtr].audio_superindex->stdindex[cur_std_idx - 1],
				                &AVI->track[audtr].audio_superindex->aIndex[cur_std_idx - 1]);

				AVI->track[audtr].audio_superindex->aIndex[cur_std_idx - 1].dwDuration =
				    AVI->track[audtr].audio_superindex->stdindex[cur_std_idx - 1]->nEntriesInUse - 1;
				if (AVI->track[audtr].a_fmt == 0x1) {
					AVI->track[audtr].audio_superindex->aIndex[cur_std_idx - 1].dwDuration *=
					    AVI->track[audtr].a_bits*AVI->track[audtr].a_rate*AVI->track[audtr].a_chans/800;
				}
			}

			// XXX: dump idx1 structure
			if (cur_std_idx == 1) {
				avi_add_chunk(AVI, (unsigned char *)"idx1", (unsigned char *)AVI->idx, AVI->n_idx*16);
				// qwBaseOffset will contain the start of the second riff chunk
			}
			// Fix the Offsets later at closing time
			avi_add_chunk(AVI, (unsigned char *)"RIFF", (unsigned char *)"AVIXLIST\0\0\0\0movi", 16);

			AVI->video_superindex->stdindex[ cur_std_idx ]->qwBaseOffset = AVI->pos -16 -8;
#ifdef DEBUG_ODML
			GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] ODML: RIFF No.%02d at Offset 0x%llX\n", cur_std_idx, AVI->pos -16 -8));
#endif

			for (audtr = 0; audtr < AVI->anum; audtr++) {
				if (AVI->track[audtr].audio_superindex)
					AVI->track[audtr].audio_superindex->stdindex[ cur_std_idx ]->qwBaseOffset =
					    AVI->pos -16 -8;

			}

			// now we can be sure
			AVI->is_opendml++;
		}

	}


	if (video) {
		avi_add_odml_index_entry_core(AVI, flags, AVI->pos, len,
		                              AVI->video_superindex->stdindex[ AVI->video_superindex->nEntriesInUse-1 ]);

		AVI->total_frames++;
	} // video

	if (audio) {
		avi_add_odml_index_entry_core(AVI, flags, AVI->pos, len,
		                              AVI->track[AVI->aptr].audio_superindex->stdindex[
		                                  AVI->track[AVI->aptr].audio_superindex->nEntriesInUse-1 ]);
	}


	return 0;
}