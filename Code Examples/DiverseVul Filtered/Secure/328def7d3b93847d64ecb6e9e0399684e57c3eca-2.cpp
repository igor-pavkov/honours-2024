GF_Err Media_GetSample(GF_MediaBox *mdia, u32 sampleNumber, GF_ISOSample **samp, u32 *sIDX, Bool no_data, u64 *out_offset)
{
	GF_Err e;
	u32 bytesRead;
	u32 dataRefIndex, chunkNumber;
	u64 offset, new_size;
	u32 sdesc_idx;
	GF_SampleEntryBox *entry;
	GF_StscEntry *stsc_entry;

	if (!mdia || !mdia->information->sampleTable) return GF_BAD_PARAM;
	if (!mdia->information->sampleTable->SampleSize)
		return GF_ISOM_INVALID_FILE;

	//OK, here we go....
	if (sampleNumber > mdia->information->sampleTable->SampleSize->sampleCount) return GF_BAD_PARAM;

	//the data info
	if (!sIDX && !no_data) return GF_BAD_PARAM;

	e = stbl_GetSampleInfos(mdia->information->sampleTable, sampleNumber, &offset, &chunkNumber, &sdesc_idx, &stsc_entry);
	if (e) return e;
	if (sIDX) (*sIDX) = sdesc_idx;

	if (out_offset) *out_offset = offset;
	if (!samp ) return GF_OK;

	if (mdia->information->sampleTable->TimeToSample) {
		//get the DTS
		e = stbl_GetSampleDTS(mdia->information->sampleTable->TimeToSample, sampleNumber, &(*samp)->DTS);
		if (e) return e;
	} else {
		(*samp)->DTS=0;
	}
	//the CTS offset
	if (mdia->information->sampleTable->CompositionOffset) {
		e = stbl_GetSampleCTS(mdia->information->sampleTable->CompositionOffset , sampleNumber, &(*samp)->CTS_Offset);
		if (e) return e;
	} else {
		(*samp)->CTS_Offset = 0;
	}
	//the size
	e = stbl_GetSampleSize(mdia->information->sampleTable->SampleSize, sampleNumber, &(*samp)->dataLength);
	if (e) return e;
	//the RAP
	if (mdia->information->sampleTable->SyncSample) {
		e = stbl_GetSampleRAP(mdia->information->sampleTable->SyncSample, sampleNumber, &(*samp)->IsRAP, NULL, NULL);
		if (e) return e;
	} else {
		//if no SyncSample, all samples are sync (cf spec)
		(*samp)->IsRAP = RAP;
	}

	if (mdia->information->sampleTable->SampleDep) {
		u32 isLeading, dependsOn, dependedOn, redundant;
		e = stbl_GetSampleDepType(mdia->information->sampleTable->SampleDep, sampleNumber, &isLeading, &dependsOn, &dependedOn, &redundant);
		if (!e) {
			if (dependsOn==1) (*samp)->IsRAP = RAP_NO;
			//commenting following code since it is wrong - an I frame is not always a SAP1, it can be a SAP2 or SAP3.
			//Keeping this code breaks AVC / HEVC openGOP import when writing sample dependencies
			//else if (dependsOn==2) (*samp)->IsRAP = RAP;

			/*if not depended upon and redundant, mark as carousel sample*/
			if ((dependedOn==2) && (redundant==1)) (*samp)->IsRAP = RAP_REDUNDANT;
			/*TODO FIXME - we must enhance the IsRAP semantics to carry disposable info ... */
		}
	}

	/*get sync shadow*/
	if (Media_IsSampleSyncShadow(mdia->information->sampleTable->ShadowSync, sampleNumber)) (*samp)->IsRAP = RAP_REDUNDANT;

	//the data info
	if (!sIDX && !no_data) return GF_BAD_PARAM;
	if (!sIDX && !out_offset) return GF_OK;
	if (!sIDX) return GF_OK;

	(*sIDX) = sdesc_idx;
//	e = stbl_GetSampleInfos(mdia->information->sampleTable, sampleNumber, &offset, &chunkNumber, sIDX, &stsc_entry);
//	if (e) return e;

	//then get the DataRef
	e = Media_GetSampleDesc(mdia, sdesc_idx, &entry, &dataRefIndex);
	if (e) return e;

	//if moov is compressed, remove offset if sample is after moov in this file
	if (mdia->mediaTrack->moov->compressed_diff) {
		GF_DataEntryBox *ent = (GF_DataEntryBox*)gf_list_get(mdia->information->dataInformation->dref->child_boxes, dataRefIndex - 1);
		if (ent && (ent->flags&1) && (offset>=mdia->mediaTrack->moov->file_offset)) {
			offset -= mdia->mediaTrack->moov->compressed_diff;
		}
	}


	if (no_data) {
		if ( ((*samp)->dataLength != 0) && mdia->mediaTrack->pack_num_samples) {
			u32 idx_in_chunk = sampleNumber - mdia->information->sampleTable->SampleToChunk->firstSampleInCurrentChunk;
			u32 left_in_chunk = stsc_entry->samplesPerChunk - idx_in_chunk;
			if (left_in_chunk > mdia->mediaTrack->pack_num_samples)
				left_in_chunk = mdia->mediaTrack->pack_num_samples;
			(*samp)->dataLength *= left_in_chunk;
			(*samp)->nb_pack = left_in_chunk;
		}
		return GF_OK;
	}

	// Open the data handler - check our mode, don't reopen in read only if this is
	//the same entry. In other modes we have no choice because the main data map is
	//divided into the original and the edition files
	if (mdia->mediaTrack->moov->mov->openMode == GF_ISOM_OPEN_READ) {
		//same as last call in read mode
		if (!mdia->information->dataHandler) {
			e = gf_isom_datamap_open(mdia, dataRefIndex, stsc_entry->isEdited);
			if (e) return e;
		}
		mdia->information->dataEntryIndex = dataRefIndex;
	} else {
		e = gf_isom_datamap_open(mdia, dataRefIndex, stsc_entry->isEdited);
		if (e) return e;
	}

	if ( mdia->mediaTrack->moov->mov->read_byte_offset || mdia->mediaTrack->moov->mov->bytes_removed) {
		GF_DataEntryBox *ent = (GF_DataEntryBox*)gf_list_get(mdia->information->dataInformation->dref->child_boxes, dataRefIndex - 1);
		if (ent && (ent->flags&1)) {
			u64 real_offset = mdia->mediaTrack->moov->mov->read_byte_offset + mdia->mediaTrack->moov->mov->bytes_removed;
			if (offset < real_offset)
				return GF_IO_ERR;

			if (mdia->information->dataHandler->last_read_offset != mdia->mediaTrack->moov->mov->read_byte_offset) {
				mdia->information->dataHandler->last_read_offset = mdia->mediaTrack->moov->mov->read_byte_offset;
				gf_bs_get_refreshed_size(mdia->information->dataHandler->bs);
			}

			offset -= real_offset;
		}
	}
	if ((*samp)->dataLength != 0) {
		if (mdia->mediaTrack->pack_num_samples) {
			u32 idx_in_chunk = sampleNumber - mdia->information->sampleTable->SampleToChunk->firstSampleInCurrentChunk;
			u32 left_in_chunk = stsc_entry->samplesPerChunk - idx_in_chunk;
			if (left_in_chunk > mdia->mediaTrack->pack_num_samples)
				left_in_chunk = mdia->mediaTrack->pack_num_samples;
			(*samp)->dataLength *= left_in_chunk;
			(*samp)->nb_pack = left_in_chunk;
		}

		/*and finally get the data, include padding if needed*/
		if ((*samp)->alloc_size) {
			if ((*samp)->alloc_size < (*samp)->dataLength + mdia->mediaTrack->padding_bytes) {
				(*samp)->data = (char *) gf_realloc((*samp)->data, sizeof(char) * ( (*samp)->dataLength + mdia->mediaTrack->padding_bytes) );
				if (! (*samp)->data) return GF_OUT_OF_MEM;

				(*samp)->alloc_size = (*samp)->dataLength + mdia->mediaTrack->padding_bytes;
			}
		} else {
			(*samp)->data = (char *) gf_malloc(sizeof(char) * ( (*samp)->dataLength + mdia->mediaTrack->padding_bytes) );
			if (! (*samp)->data) return GF_OUT_OF_MEM;
		}
		if (mdia->mediaTrack->padding_bytes)
			memset((*samp)->data + (*samp)->dataLength, 0, sizeof(char) * mdia->mediaTrack->padding_bytes);

		//check if we can get the sample (make sure we have enougth data...)
		new_size = gf_bs_get_size(mdia->information->dataHandler->bs);
		if (offset + (*samp)->dataLength > new_size) {
			//always refresh the size to avoid wrong info on http/ftp
			new_size = gf_bs_get_refreshed_size(mdia->information->dataHandler->bs);
			if (offset + (*samp)->dataLength > new_size) {
				mdia->BytesMissing = offset + (*samp)->dataLength - new_size;
				return GF_ISOM_INCOMPLETE_FILE;
			}
		}

		bytesRead = gf_isom_datamap_get_data(mdia->information->dataHandler, (*samp)->data, (*samp)->dataLength, offset);
		//if bytesRead != sampleSize, we have an IO err
		if (bytesRead < (*samp)->dataLength) {
			return GF_IO_ERR;
		}
		mdia->BytesMissing = 0;
	}

	//finally rewrite the sample if this is an OD Access Unit or NAL-based one
	//we do this even if sample size is zero because of sample implicit reconstruction rules (especially tile tracks)
	if (mdia->handler->handlerType == GF_ISOM_MEDIA_OD) {
		if (!mdia->mediaTrack->moov->mov->disable_odf_translate) {
			e = Media_RewriteODFrame(mdia, *samp);
			if (e) return e;
		}
	}
	else if (gf_isom_is_nalu_based_entry(mdia, entry)
		&& !gf_isom_is_encrypted_entry(entry->type)
	) {
		e = gf_isom_nalu_sample_rewrite(mdia, *samp, sampleNumber, (GF_MPEGVisualSampleEntryBox *)entry);
		if (e) return e;
	}
	else if (mdia->mediaTrack->moov->mov->convert_streaming_text
	         && ((mdia->handler->handlerType == GF_ISOM_MEDIA_TEXT) || (mdia->handler->handlerType == GF_ISOM_MEDIA_SCENE) || (mdia->handler->handlerType == GF_ISOM_MEDIA_SUBT))
	         && (entry->type == GF_ISOM_BOX_TYPE_TX3G || entry->type == GF_ISOM_BOX_TYPE_TEXT)
	        ) {
		u64 dur;
		if (sampleNumber == mdia->information->sampleTable->SampleSize->sampleCount) {
			dur = mdia->mediaHeader->duration - (*samp)->DTS;
		} else {
			stbl_GetSampleDTS(mdia->information->sampleTable->TimeToSample, sampleNumber+1, &dur);
			dur -= (*samp)->DTS;
		}
		e = gf_isom_rewrite_text_sample(*samp, sdesc_idx, (u32) dur);
		if (e) return e;
	}
	return GF_OK;
}