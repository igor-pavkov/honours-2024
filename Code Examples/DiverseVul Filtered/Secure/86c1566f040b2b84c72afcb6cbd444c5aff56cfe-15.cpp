GF_Err gf_isom_rtp_packet_set_flags(GF_ISOFile *the_file, u32 trackNumber,
                                    u8 PackingBit,
                                    u8 eXtensionBit,
                                    u8 MarkerBit,
                                    u8 disposable_packet,
                                    u8 IsRepeatedPacket)
{
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *entry;
	GF_RTPPacket *pck;
	u32 dataRefIndex, ind;
	GF_Err e;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !CheckHintFormat(trak, GF_ISOM_HINT_RTP)) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, trak->Media->information->sampleTable->currentEntryIndex, (GF_SampleEntryBox **) &entry, &dataRefIndex);
	if (e) return e;
	if (!entry->hint_sample) return GF_BAD_PARAM;

	ind = gf_list_count(entry->hint_sample->packetTable);
	if (!ind) return GF_BAD_PARAM;
	pck = (GF_RTPPacket *)gf_list_get(entry->hint_sample->packetTable, ind-1);

	pck->P_bit = PackingBit ? 1 : 0;
	pck->X_bit = eXtensionBit ? 1 : 0;
	pck->M_bit = MarkerBit ? 1 : 0;
	pck->B_bit = disposable_packet ? 1 : 0;
	pck->R_bit = IsRepeatedPacket ? 1 : 0;
	return GF_OK;
}