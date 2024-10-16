GF_Err DoWrite(MovieWriter *mw, GF_List *writers, GF_BitStream *bs, u8 Emulation, u64 StartOffset)
{
	u32 i;
	GF_Err e;
	TrackWriter *writer;
	u64 offset, sampOffset, predOffset;
	u32 chunkNumber, descIndex, sampSize;
	Bool force;
	GF_StscEntry *stsc_ent;
	u64 size, mdatSize = 0;
	GF_ISOFile *movie = mw->movie;

	/*write meta content first - WE DON'T support fragmentation of resources in ISOM atm*/
	if (movie->openMode != GF_ISOM_OPEN_WRITE) {
		if (movie->meta) {
			e = DoWriteMeta(movie, movie->meta, bs, Emulation, StartOffset, &size);
			if (e) return e;
			mdatSize += size;
			StartOffset += size;
		}
		if (movie->moov && movie->moov->meta) {
			e = DoWriteMeta(movie, movie->meta, bs, Emulation, StartOffset, &size);
			if (e) return e;
			mdatSize += size;
			StartOffset += size;
		}
		i=0;
		while ((writer = (TrackWriter*)gf_list_enum(writers, &i))) {
			if (writer->mdia->mediaTrack->meta) {
				e = DoWriteMeta(movie, movie->meta, bs, Emulation, StartOffset, &size);
				if (e) return e;
				mdatSize += size;
				StartOffset += size;
			}
		}
	}

	offset = StartOffset;
	predOffset = 0;
	i=0;
	while ((writer = (TrackWriter*)gf_list_enum(writers, &i))) {
		while (!writer->isDone) {
			Bool self_contained;
			u32 nb_samp=1;
			//To Check: are empty sample tables allowed ???
			if (writer->sampleNumber > writer->stbl->SampleSize->sampleCount) {
				writer->isDone = 1;
				continue;
			}
			e = stbl_GetSampleInfos(writer->stbl, writer->sampleNumber, &sampOffset, &chunkNumber, &descIndex, &stsc_ent);
			if (e) return e;
			e = stbl_GetSampleSize(writer->stbl->SampleSize, writer->sampleNumber, &sampSize);
			if (e) return e;

			update_writer_constant_dur(movie, writer, stsc_ent, &nb_samp, &sampSize, GF_TRUE);

			//update our chunks.
			force = 0;
			if (movie->openMode == GF_ISOM_OPEN_WRITE) {
				offset = sampOffset;
				if (predOffset != offset)
					force = 1;
			}

			if (writer->stbl->MaxChunkSize && (writer->chunkSize + sampSize > writer->stbl->MaxChunkSize)) {
				writer->chunkSize = 0;
				force = 1;
			}
			writer->chunkSize += sampSize;

			self_contained = ((writer->all_dref_mode==ISOM_DREF_SELF) || Media_IsSelfContained(writer->mdia, descIndex) ) ? GF_TRUE : GF_FALSE;

			//update our global offset...
			if (self_contained) {
				e = stbl_SetChunkAndOffset(writer->stbl, writer->sampleNumber, descIndex, writer->stsc, &writer->stco, offset, force, nb_samp);
				if (e) return e;
				if (movie->openMode == GF_ISOM_OPEN_WRITE) {
					predOffset = sampOffset + sampSize;
				} else {
					offset += sampSize;
					mdatSize += sampSize;
				}
			} else {
				if (predOffset != offset) force = 1;
				predOffset = sampOffset + sampSize;
				//we have a DataRef, so use the offset idicated in sampleToChunk and ChunkOffset tables...
				e = stbl_SetChunkAndOffset(writer->stbl, writer->sampleNumber, descIndex, writer->stsc, &writer->stco, sampOffset, force, nb_samp);
				if (e) return e;
			}
			//we write the sample if not emulation
			if (!Emulation) {
				if (self_contained) {
					e = WriteSample(mw, sampSize, sampOffset, stsc_ent->isEdited, bs, 1);
					if (e) return e;
				}
			}
			//ok, the track is done
			if (writer->sampleNumber >= writer->stbl->SampleSize->sampleCount) {
				writer->isDone = 1;
			} else {
				writer->sampleNumber += nb_samp;
			}
		}
	}
	//set the mdatSize...
	movie->mdat->dataSize = mdatSize;
	return GF_OK;
}