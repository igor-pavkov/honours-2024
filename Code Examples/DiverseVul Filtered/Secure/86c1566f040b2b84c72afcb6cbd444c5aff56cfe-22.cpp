GF_Err gf_isom_sdp_clean(GF_ISOFile *movie)
{
	GF_UserDataMap *map;
	GF_HintTrackInfoBox *hnti;

	//check if we have a udta ...
	if (!movie->moov || !movie->moov->udta) return GF_OK;

	//find a hnti in the udta
	map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_HNTI, NULL);
	if (!map) return GF_OK;

	//there should be one and only one hnti
	if (gf_list_count(map->boxes) != 1) return GF_ISOM_INVALID_FILE;
	hnti = (GF_HintTrackInfoBox *)gf_list_get(map->boxes, 0);

	//remove and destroy the entry
	gf_list_rem(map->boxes, 0);
	gf_isom_box_del((GF_Box *)hnti);
	return GF_OK;
}