GF_Err gf_isom_hint_sample_description_data(GF_ISOFile *the_file, u32 trackNumber, GF_ISOTrackID SourceTrackID, u32 StreamDescriptionIndex, u16 DataLength, u32 offsetInDescription, u8 AtBegin)
{
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *entry;
	u32 count;
	u16 refIndex;
	GF_HintPacket *pck;
	GF_StreamDescDTE *dte;
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

	dte = (GF_StreamDescDTE *) NewDTE(3);
	dte->byteOffset = offsetInDescription;
	dte->dataLength = DataLength;
	dte->streamDescIndex = StreamDescriptionIndex;
	if (SourceTrackID == trak->Header->trackID) {
		dte->trackRefIndex = (s8) -1;
	} else {
		//get (or set) the track reference index
		e = Track_FindRef(trak, GF_ISOM_REF_HINT, &hint);
		if (e) return e;
		e = reftype_AddRefTrack(hint, SourceTrackID, &refIndex);
		if (e) return e;
		//WARNING: IN QT, MUST BE 0-based !!!
		dte->trackRefIndex = (u8) (refIndex - 1);
	}
	return gf_isom_hint_pck_add_dte(pck, (GF_GenericDTE *)dte, AtBegin);
}