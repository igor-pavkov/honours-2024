GF_Err gf_isom_sdp_track_get(GF_ISOFile *the_file, u32 trackNumber, const char **sdp, u32 *length)
{
	GF_TrackBox *trak;
	GF_UserDataMap *map;
	GF_HintTrackInfoBox *hnti;
	GF_SDPBox *sdpa;

	*sdp = NULL;
	*length = 0;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->udta) return GF_OK;

	map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_HNTI, NULL);
	if (!map) return GF_ISOM_INVALID_FILE;

	//we should have only one HNTI in the UDTA
	if (gf_list_count(map->boxes) != 1) return GF_ISOM_INVALID_FILE;

	hnti = (GF_HintTrackInfoBox *)gf_list_get(map->boxes, 0);
	if (!hnti->SDP) return GF_OK;
	sdpa = (GF_SDPBox *) hnti->SDP;

	*length = (u32) strlen(sdpa->sdpText);
	*sdp = sdpa->sdpText;
	return GF_OK;
}