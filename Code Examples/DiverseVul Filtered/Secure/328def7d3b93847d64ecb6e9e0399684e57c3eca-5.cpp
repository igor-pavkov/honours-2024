GF_Err Media_SetDuration(GF_TrackBox *trak)
{
	GF_Err e;
	GF_ESD *esd;
	u64 DTS;
	GF_SttsEntry *ent;
	u32 nbSamp;

	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable)
		return GF_ISOM_INVALID_FILE;

	if (!trak->Media->information->sampleTable->SampleSize || !trak->Media->information->sampleTable->TimeToSample)
		return GF_ISOM_INVALID_FILE;

	nbSamp = trak->Media->information->sampleTable->SampleSize->sampleCount;

	//we need to check how many samples we have.
	// == 1 -> last sample duration == default duration
	// > 1 -> last sample duration == prev sample duration
	switch (nbSamp) {
	case 0:
		trak->Media->mediaHeader->duration = 0;
		if (Track_IsMPEG4Stream(trak->Media->handler->handlerType)) {
			Media_GetESD(trak->Media, 1, &esd, 1);
			if (esd && esd->URLString) trak->Media->mediaHeader->duration = (u64) -1;

		}
		return GF_OK;

//	case 1:
//		trak->Media->mediaHeader->duration = trak->Media->mediaHeader->timeScale;
//		return GF_OK;

	default:
		//we assume a constant frame rate for the media and assume the last sample
		//will be hold the same time as the prev one
		e = stbl_GetSampleDTS(trak->Media->information->sampleTable->TimeToSample, nbSamp, &DTS);
		if (e < 0) {
			return e;
		}
		if (trak->Media->information->sampleTable->TimeToSample->nb_entries > 0) {
			ent = &trak->Media->information->sampleTable->TimeToSample->entries[trak->Media->information->sampleTable->TimeToSample->nb_entries-1];
		} else {
			ent = NULL;
		}
		trak->Media->mediaHeader->duration = DTS;


#if 1
		if (ent) trak->Media->mediaHeader->duration += ent->sampleDelta;
#else
		if (!ent) {
			u64 DTSprev;
			stbl_GetSampleDTS(trak->Media->information->sampleTable->TimeToSample, nbSamp-1, &DTSprev);
			trak->Media->mediaHeader->duration += (DTS - DTSprev);
		} else {
#ifndef GPAC_DISABLE_ISOM_WRITE
			if (trak->moov->mov->editFileMap && trak->Media->information->sampleTable->CompositionOffset) {
				u32 count, i;
				u64 max_ts;
				GF_DttsEntry *cts_ent;
				GF_CompositionOffsetBox *ctts = trak->Media->information->sampleTable->CompositionOffset;
				if (ctts->w_LastSampleNumber==nbSamp) {
					count = gf_list_count(ctts->entryList);
					max_ts = trak->Media->mediaHeader->duration;
					while (count) {
						count -= 1;
						cts_ent = gf_list_get(ctts->entryList, count);
						if (nbSamp<cts_ent->sampleCount) break;

						for (i=0; i<cts_ent->sampleCount; i++) {
							stbl_GetSampleDTS(trak->Media->information->sampleTable->TimeToSample, nbSamp-i, &DTS);
							if ((s32) cts_ent->decodingOffset < 0) max_ts = DTS;
							else max_ts = DTS + cts_ent->decodingOffset;
							if (max_ts>=trak->Media->mediaHeader->duration) {
								trak->Media->mediaHeader->duration = max_ts;
							} else {
								break;
							}
						}
						if (max_ts<trak->Media->mediaHeader->duration) {
							break;
						}
						nbSamp-=cts_ent->sampleCount;
					}
				}
			}
#endif /*GPAC_DISABLE_ISOM_WRITE*/
			trak->Media->mediaHeader->duration += ent->sampleDelta;
		}
#endif
		return GF_OK;
	}
}