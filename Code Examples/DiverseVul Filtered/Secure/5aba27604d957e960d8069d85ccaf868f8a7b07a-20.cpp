GF_Err DoInterleave(MovieWriter *mw, GF_List *writers, GF_BitStream *bs, u8 Emulation, u64 StartOffset, Bool drift_inter)
{
	u32 i, tracksDone;
	TrackWriter *tmp, *curWriter;
	GF_Err e;
	u32 descIndex, sampSize, chunkNumber;
	u64 DTS;
	u32 moov_timescale;
	u16 curGroupID;
	Bool forceNewChunk, writeGroup;
	GF_StscEntry *stsc_ent;
	//this is used to emulate the write ...
	u64 offset, sampOffset, size, mdatSize;
	u32 count;
	GF_ISOFile *movie = mw->movie;

	mdatSize = 0;

#ifdef TEST_LARGE_FILES
	if (!Emulation) {
		char *blank;
		u32 count, i;
		i = count = 0;
		blank = gf_malloc(sizeof(char)*1024*1024);
		memset(blank, 0, sizeof(char)*1024*1024);
		count = 4096;
		memset(blank, 0, sizeof(char)*1024*1024);
		while (i<count) {
			u32 res = gf_bs_write_data(bs, blank, 1024*1024);
			if (res != 1024*1024) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("error writing to disk: only %d bytes written\n", res));
			}
			i++;
			GF_LOG(GF_LOG_ERROR, GF_LOG_CORE, ("writing blank block: %.02f done - %d/%d \r", (100.0*i)/count , i, count));
		}
		gf_free(blank);
	}
	mdatSize = 4096*1024;
	mdatSize *= 1024;
#endif

	/*write meta content first - WE DON'T support fragmentation of resources in ISOM atm*/
	if (movie->meta) {
		e = DoWriteMeta(movie, movie->meta, bs, Emulation, StartOffset, &size);
		if (e) return e;
		mdatSize += size;
		StartOffset += (u32) size;
	}
	if (movie->moov) {
		if (movie->moov->meta) {
			e = DoWriteMeta(movie, movie->moov->meta, bs, Emulation, StartOffset, &size);
			if (e) return e;
			mdatSize += size;
			StartOffset += (u32) size;
		}
		i=0;
		while ((tmp = (TrackWriter*)gf_list_enum(writers, &i))) {
			if (tmp->mdia->mediaTrack->meta) {
				e = DoWriteMeta(movie, tmp->mdia->mediaTrack->meta, bs, Emulation, StartOffset, &size);
				if (e) return e;
				mdatSize += size;
				StartOffset += (u32) size;
			}
		}
	}


	if (movie->storageMode == GF_ISOM_STORE_TIGHT)
		return DoFullInterleave(mw, writers, bs, Emulation, StartOffset);

	curGroupID = 1;
	//we emulate a write from this offset...
	offset = StartOffset;
	tracksDone = 0;

#ifdef TEST_LARGE_FILES
	offset += mdatSize;
#endif

	moov_timescale = movie->moov && movie->moov->mvhd ? movie->moov->mvhd->timeScale : 1000;

	count = gf_list_count(writers);
	//browse each groups
	while (1) {
		/*the max DTS the chunk of the current writer*/
		u64 chunkLastDTS = 0;
		/*the timescale related to the max DTS*/
		u32 chunkLastScale = 0;

		writeGroup = 1;

		//proceed a group
		while (writeGroup) {
			curWriter = NULL;
			for (i=0 ; i < count; i++) {
				tmp = (TrackWriter*)gf_list_get(writers, i);

				//is it done writing ?
				if (tmp->isDone) continue;

				//is it in our group ??
				if (tmp->stbl->groupID != curGroupID) continue;

				//write till this chunk is full on this track...
				while (1) {
					Bool self_contained;
					u32 nb_samp = 1;
					u32 sample_dur;
					u64 chunk_prev_dur;
					//To Check: are empty sample tables allowed ???
					if (tmp->sampleNumber > tmp->stbl->SampleSize->sampleCount) {
						tmp->isDone = 1;
						tracksDone ++;
						break;
					}

					//OK, get the current sample in this track
					stbl_GetSampleDTS_and_Duration(tmp->stbl->TimeToSample, tmp->sampleNumber, &DTS, &sample_dur);

					//can this sample fit in our chunk ?
					if ( ( (DTS - tmp->DTSprev) + tmp->chunkDur) * moov_timescale > movie->interleavingTime * tmp->timeScale
					        /*drift check: reject sample if outside our check window*/
					        || (drift_inter && chunkLastDTS && ( ((u64)tmp->DTSprev*chunkLastScale) > ((u64)chunkLastDTS*tmp->timeScale)) )
					   ) {
						//in case the sample is longer than InterleaveTime
						if (!tmp->chunkDur) {
							forceNewChunk = 1;
						} else {
							//this one is full. go to next one (exit the loop)
							tmp->chunkDur = 0;
							//forceNewChunk = 0;
							break;
						}
					} else {
						forceNewChunk = tmp->chunkDur ? 0 : 1;
					}
					//OK, we can write this track
					curWriter = tmp;

					//small check for first 2 samples (DTS = 0)
					//only in the old mode can chunkdur be 0 for dts 0
					if (tmp->sampleNumber == 2 && !tmp->chunkDur && gf_sys_old_arch_compat() ) {
						forceNewChunk = 0;
					}

					chunk_prev_dur = tmp->chunkDur;
					//FIXME we do not apply patch in test mode for now since this breaks all our hashes, remove this
					//once we move to filters permanently
					if (!gf_sys_old_arch_compat()) {
						tmp->chunkDur += sample_dur;
					} else {
						//old style, compute based on DTS diff
						tmp->chunkDur += (u32) (DTS - tmp->DTSprev);
					}
					tmp->DTSprev = DTS;

					e = stbl_GetSampleInfos(curWriter->stbl, curWriter->sampleNumber, &sampOffset, &chunkNumber, &descIndex, &stsc_ent);
					if (e)
						return e;
					e = stbl_GetSampleSize(curWriter->stbl->SampleSize, curWriter->sampleNumber, &sampSize);
					if (e)
						return e;

					self_contained = ((curWriter->all_dref_mode==ISOM_DREF_SELF) || Media_IsSelfContained(curWriter->mdia, descIndex)) ? GF_TRUE : GF_FALSE;

					update_writer_constant_dur(movie, curWriter, stsc_ent, &nb_samp, &sampSize, GF_FALSE);

					if (curWriter->stbl->MaxChunkSize && (curWriter->chunkSize + sampSize > curWriter->stbl->MaxChunkSize)) {
						curWriter->chunkSize = 0;
						tmp->chunkDur -= chunk_prev_dur;
						forceNewChunk = 1;
					}
					curWriter->chunkSize += sampSize;

					//do we actually write, or do we emulate ?
					if (Emulation) {
						//update our offsets...
						if (self_contained) {
							e = stbl_SetChunkAndOffset(curWriter->stbl, curWriter->sampleNumber, descIndex, curWriter->stsc, &curWriter->stco, offset, forceNewChunk, nb_samp);
							if (e)
								return e;
							offset += sampSize;
							mdatSize += sampSize;
						} else {
							if (curWriter->prev_offset != sampOffset) forceNewChunk = 1;
							curWriter->prev_offset = sampOffset + sampSize;

							//we have a DataRef, so use the offset idicated in sampleToChunk
							//and ChunkOffset tables...
							e = stbl_SetChunkAndOffset(curWriter->stbl, curWriter->sampleNumber, descIndex, curWriter->stsc, &curWriter->stco, sampOffset, forceNewChunk, nb_samp);
							if (e) return e;
						}
					} else {
						//we're writing ....
						if (self_contained) {
							e = WriteSample(mw, sampSize, sampOffset, stsc_ent->isEdited, bs, nb_samp);
							if (e)
								return e;
						}
					}
					//ok, the sample is done
					if (curWriter->sampleNumber >= curWriter->stbl->SampleSize->sampleCount) {
						curWriter->isDone = 1;
						//one more track done...
						tracksDone ++;
						break;
					} else {
						curWriter->sampleNumber += nb_samp;
					}
				}
				/*record chunk end-time & track timescale for drift-controled interleaving*/
				if (drift_inter && curWriter) {
					chunkLastScale = curWriter->timeScale;
					chunkLastDTS = curWriter->DTSprev;
					/*add one interleave window drift - since the "maxDTS" is the previously written one, we will
					have the following cases:
					- sample doesn't fit: post-pone and force new chunk
					- sample time larger than previous chunk time + interleave: post-pone and force new chunk
					- otherwise store and track becomes current reference

					this ensures a proper drift regulation (max DTS diff is less than the interleaving window)
					*/
					chunkLastDTS += curWriter->timeScale * movie->interleavingTime / moov_timescale;
				}
			}
			//no sample found, we're done with this group
			if (!curWriter) {
				writeGroup = 0;
				continue;
			}
		}
		//if all our track are done, break
		if (tracksDone == gf_list_count(writers)) break;
		//go to next group
		curGroupID ++;
	}
	if (movie->mdat) movie->mdat->dataSize = mdatSize;
	return GF_OK;
}