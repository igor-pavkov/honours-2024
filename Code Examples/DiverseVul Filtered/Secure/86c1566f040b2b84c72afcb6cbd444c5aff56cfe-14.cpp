GF_Err gf_isom_rtp_packet_begin(GF_ISOFile *the_file, u32 trackNumber,
                                s32 relativeTime,
                                u8 PackingBit,
                                u8 eXtensionBit,
                                u8 MarkerBit,
                                u8 PayloadType,
                                u8 B_frame,
                                u8 IsRepeatedPacket,
                                u16 SequenceNumber)
{
	GF_TrackBox *trak;
	GF_HintSampleEntryBox *entry;
	GF_RTPPacket *pck;
	u32 dataRefIndex;
	GF_Err e;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !CheckHintFormat(trak, GF_ISOM_HINT_RTP)) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, trak->Media->information->sampleTable->currentEntryIndex, (GF_SampleEntryBox **) &entry, &dataRefIndex);
	if (e) return e;
	if (!entry->hint_sample) return GF_BAD_PARAM;

	pck = (GF_RTPPacket *) gf_isom_hint_pck_new(entry->type);

	pck->P_bit = PackingBit ? 1 : 0;
	pck->X_bit = eXtensionBit ? 1 : 0;
	pck->M_bit = MarkerBit ? 1 : 0;
	pck->payloadType = PayloadType;
	pck->SequenceNumber = SequenceNumber;
	pck->B_bit = B_frame ? 1 : 0;
	pck->R_bit = IsRepeatedPacket ? 1 : 0;
	pck->relativeTransTime = relativeTime;
	return gf_list_add(entry->hint_sample->packetTable, pck);
}