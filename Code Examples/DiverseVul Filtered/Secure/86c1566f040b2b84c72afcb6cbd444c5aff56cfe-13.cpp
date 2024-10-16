GF_Err gf_isom_sdp_get(GF_ISOFile *movie, const char **sdp, u32 *length)
{
	GF_UserDataMap *map;
	GF_HintTrackInfoBox *hnti;
	GF_RTPBox *rtp;
	*length = 0;
	*sdp = NULL;
	if (!movie || !movie->moov) return GF_BAD_PARAM;
	//check if we have a udta ...
	if (!movie->moov->udta) return GF_OK;

	//find a hnti in the udta
	map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_HNTI, NULL);
	if (!map) return GF_OK;

	//there should be one and only one hnti
	if (gf_list_count(map->boxes) != 1) return GF_ISOM_INVALID_FILE;
	hnti = (GF_HintTrackInfoBox *)gf_list_get(map->boxes, 0);

	if (!hnti->SDP) return GF_OK;
	rtp = (GF_RTPBox *) hnti->SDP;

	*length = (u32) strlen(rtp->sdpText);
	*sdp = rtp->sdpText;
	return GF_OK;
}