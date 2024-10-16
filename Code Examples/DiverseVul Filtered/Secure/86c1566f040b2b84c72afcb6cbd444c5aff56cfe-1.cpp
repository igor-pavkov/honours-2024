GF_Err gf_isom_sdp_clean_track(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	GF_UserDataMap *map;
	GF_HintTrackInfoBox *hnti;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	//currently, only RTP hinting supports SDP
	if (!CheckHintFormat(trak, GF_ISOM_HINT_RTP)) return GF_BAD_PARAM;

	map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_HNTI, NULL);
	if (!map) return GF_ISOM_INVALID_FILE;

	//we should have only one HNTI in the UDTA
	if (gf_list_count(map->boxes) != 1) return GF_ISOM_INVALID_FILE;

	hnti = (GF_HintTrackInfoBox *)gf_list_get(map->boxes, 0);
	if (!hnti->SDP) return GF_OK;
	//and free the SDP
	gf_free(((GF_SDPBox *)hnti->SDP)->sdpText);
	((GF_SDPBox *)hnti->SDP)->sdpText = NULL;
	return GF_OK;
}