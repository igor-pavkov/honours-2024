GF_Err gf_isom_rtp_packet_set_offset(GF_ISOFile *the_file, u32 trackNumber, s32 timeOffset)
{
	GF_RTPOBox *rtpo;
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *entry;
	GF_RTPPacket *pck;
	u32 dataRefIndex, i;
	GF_Err e;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !CheckHintFormat(trak, GF_ISOM_HINT_RTP)) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, trak->Media->information->sampleTable->currentEntryIndex, (GF_SampleEntryBox **) &entry, &dataRefIndex);
	if (e) return e;
	if (!entry->hint_sample) return GF_BAD_PARAM;

	pck = (GF_RTPPacket *)gf_list_get(entry->hint_sample->packetTable, gf_list_count(entry->hint_sample->packetTable) - 1);
	if (!pck) return GF_BAD_PARAM;

	//look in the TLV
	i=0;
	while ((rtpo = (GF_RTPOBox *)gf_list_enum(pck->TLV, &i))) {
		if (rtpo->type == GF_ISOM_BOX_TYPE_RTPO) {
			rtpo->timeOffset = timeOffset;
			return GF_OK;
		}
	}
	//not found, add it
	rtpo = (GF_RTPOBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_RTPO);
	if (!rtpo) return GF_OUT_OF_MEM;
	rtpo->timeOffset = timeOffset;

	return gf_list_add(pck->TLV, rtpo);
}