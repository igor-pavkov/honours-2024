GF_Err gf_isom_hint_sample_data(GF_ISOFile *the_file, u32 trackNumber, GF_ISOTrackID SourceTrackID, u32 SampleNumber, u16 DataLength, u32 offsetInSample, u8 *extra_data, u8 AtBegin)
{
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *entry;
	u32 count;
	u16 refIndex;
	GF_HintPacket *pck;
	GF_SampleDTE *dte;
	GF_Err e;
	GF_TrackReferenceTypeBox *hint;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !IsHintTrack(trak)) return GF_BAD_PARAM;


	e = Media_GetSampleDesc(trak->Media, trak->Media->information->sampleTable->currentEntryIndex, (GF_SampleEntryBox **) &entry, &count);
	if (e) return e;
	if (!entry->hint_sample) return GF_BAD_PARAM;
	count = gf_list_count(entry->hint_sample->packetTable);
	if (!count) return GF_BAD_PARAM;
	pck = (GF_HintPacket *)gf_list_get(entry->hint_sample->packetTable, count - 1);

	dte = (GF_SampleDTE *) NewDTE(2);

	dte->dataLength = DataLength;
	dte->sampleNumber = SampleNumber;
	dte->byteOffset = offsetInSample;

	//we're getting data from another track
	if (SourceTrackID != trak->Header->trackID) {
		//get (or set) the track reference index
		e = Track_FindRef(trak, GF_ISOM_REF_HINT, &hint);
		if (e) return e;
		e = reftype_AddRefTrack(hint, SourceTrackID, &refIndex);
		if (e) return e;
		//WARNING: IN QT, MUST BE 0-based !!!
		dte->trackRefIndex = (u8) (refIndex - 1);
	} else {
		//we're in the hint track
		dte->trackRefIndex = (s8) -1;
		//basic check...
		if (SampleNumber > trak->Media->information->sampleTable->SampleSize->sampleCount + 1) {
			DelDTE((GF_GenericDTE *)dte);
			return GF_BAD_PARAM;
		}

		//are we in the current sample ??
		if (!SampleNumber || (SampleNumber == trak->Media->information->sampleTable->SampleSize->sampleCount + 1)) {
			//we adding some stuff in the current sample ...
			dte->byteOffset += entry->hint_sample->dataLength;
			entry->hint_sample->AdditionalData = (char*)gf_realloc(entry->hint_sample->AdditionalData, sizeof(char) * (entry->hint_sample->dataLength + DataLength));
			if (AtBegin) {
				if (entry->hint_sample->dataLength)
					memmove(entry->hint_sample->AdditionalData + DataLength, entry->hint_sample->AdditionalData, entry->hint_sample->dataLength);
				memcpy(entry->hint_sample->AdditionalData, extra_data, DataLength);
				/*offset existing DTE*/
				gf_isom_hint_pck_offset(pck, DataLength, SampleNumber);
			} else {
				memcpy(entry->hint_sample->AdditionalData + entry->hint_sample->dataLength, extra_data, DataLength);
			}
			entry->hint_sample->dataLength += DataLength;
			//and set the sample number ...
			dte->sampleNumber = trak->Media->information->sampleTable->SampleSize->sampleCount + 1;
		}
	}
	//OK, add the entry
	return gf_isom_hint_pck_add_dte(pck, (GF_GenericDTE *)dte, AtBegin);
}