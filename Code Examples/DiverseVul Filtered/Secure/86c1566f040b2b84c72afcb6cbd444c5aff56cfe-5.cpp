GF_Err gf_isom_hint_direct_data(GF_ISOFile *the_file, u32 trackNumber, u8 *data, u32 dataLength, u8 AtBegin)
{
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *entry;
	u32 count;
	GF_HintPacket *pck;
	GF_ImmediateDTE *dte;
	GF_Err e;
	u32 offset = 0;

	if (!dataLength) return GF_OK;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !IsHintTrack(trak) || (dataLength > 14)) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, trak->Media->information->sampleTable->currentEntryIndex, (GF_SampleEntryBox **) &entry, &count);
	if (e) return e;
	if (!entry->hint_sample) return GF_BAD_PARAM;
	count = gf_list_count(entry->hint_sample->packetTable);
	if (!count) return GF_BAD_PARAM;
	pck = (GF_HintPacket *)gf_list_get(entry->hint_sample->packetTable, count - 1);

	dte = (GF_ImmediateDTE *) NewDTE(1);
	memcpy(dte->data, data + offset, dataLength);
	dte->dataLength = dataLength;
	return gf_isom_hint_pck_add_dte(pck, (GF_GenericDTE *)dte, AtBegin);
}