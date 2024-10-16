GF_Err gf_isom_sdp_add_line(GF_ISOFile *movie, const char *text)
{
	GF_UserDataMap *map;
	GF_RTPBox *rtp;
	GF_Err e;
	GF_HintTrackInfoBox *hnti;
	char *buf;

	if (!movie->moov) return GF_BAD_PARAM;

	//check if we have a udta ...
	if (!movie->moov->udta) {
		e = moov_on_child_box((GF_Box*)movie->moov, gf_isom_box_new_parent(&movie->moov->child_boxes, GF_ISOM_BOX_TYPE_UDTA), GF_FALSE);
		if (e) return e;
	}
	//find a hnti in the udta
	map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_HNTI, NULL);
	if (!map) {
		e = udta_on_child_box((GF_Box *)movie->moov->udta, gf_isom_box_new(GF_ISOM_BOX_TYPE_HNTI), GF_FALSE);
		if (e) return e;
		map = udta_getEntry(movie->moov->udta, GF_ISOM_BOX_TYPE_HNTI, NULL);
	}

	//there should be one and only one hnti
	if (!gf_list_count(map->boxes) ) {
		e = udta_on_child_box((GF_Box *)movie->moov->udta, gf_isom_box_new(GF_ISOM_BOX_TYPE_HNTI), GF_FALSE);
		if (e) return e;
	}
	else if (gf_list_count(map->boxes) < 1) return GF_ISOM_INVALID_FILE;

	hnti = (GF_HintTrackInfoBox *)gf_list_get(map->boxes, 0);

	if (!hnti->SDP) {
		GF_Box *a = gf_isom_box_new_ex(GF_ISOM_BOX_TYPE_RTP, GF_ISOM_BOX_TYPE_HNTI, 0, GF_FALSE);
		if (!a) return GF_OUT_OF_MEM;
		hnti_on_child_box((GF_Box*)hnti, a, GF_FALSE);
		if (!hnti->child_boxes) hnti->child_boxes = gf_list_new();
		gf_list_add(hnti->child_boxes, a);
	}
	rtp = (GF_RTPBox *) hnti->SDP;

	if (!rtp->sdpText) {
		rtp->sdpText = (char*)gf_malloc(sizeof(char) * (strlen(text) + 3));
		if (!rtp->sdpText) return GF_OUT_OF_MEM;

		strcpy(rtp->sdpText, text);
		strcat(rtp->sdpText, "\r\n");
		return GF_OK;
	}
	buf = (char*)gf_malloc(sizeof(char) * (strlen(rtp->sdpText) + strlen(text) + 3));
	if (!buf) return GF_OUT_OF_MEM;
	
	strcpy(buf, rtp->sdpText);
	strcat(buf, text);
	strcat(buf, "\r\n");
	gf_free(rtp->sdpText);
	ReorderSDP(buf, GF_TRUE);
	rtp->sdpText = buf;
	return GF_OK;
}