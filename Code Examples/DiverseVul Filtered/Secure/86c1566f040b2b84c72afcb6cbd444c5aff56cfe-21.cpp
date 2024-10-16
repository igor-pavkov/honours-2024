GF_Err gf_isom_sdp_add_track_line(GF_ISOFile *the_file, u32 trackNumber, const char *text)
{
	GF_TrackBox *trak;
	GF_UserDataMap *map;
	GF_HintTrackInfoBox *hnti;
	GF_SDPBox *sdp;
	GF_Err e;
	char *buf;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	//currently, only RTP hinting supports SDP
	if (!CheckHintFormat(trak, GF_ISOM_HINT_RTP)) return GF_BAD_PARAM;

	map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_HNTI, NULL);
	if (!map) return GF_ISOM_INVALID_FILE;

	//we should have only one HNTI in the UDTA
	if (gf_list_count(map->boxes) != 1) return GF_ISOM_INVALID_FILE;

	hnti = (GF_HintTrackInfoBox *)gf_list_get(map->boxes, 0);
	if (!hnti->SDP) {
		e = hnti_on_child_box((GF_Box*)hnti, gf_isom_box_new_parent(&hnti->child_boxes, GF_ISOM_BOX_TYPE_SDP), GF_FALSE);
		if (e) return e;
	}
	sdp = (GF_SDPBox *) hnti->SDP;

	if (!sdp->sdpText) {
		sdp->sdpText = (char *)gf_malloc(sizeof(char) * (strlen(text) + 3));
		if (!sdp->sdpText) return GF_OUT_OF_MEM;

		strcpy(sdp->sdpText, text);
		strcat(sdp->sdpText, "\r\n");
		return GF_OK;
	}
	buf = (char *)gf_malloc(sizeof(char) * (strlen(sdp->sdpText) + strlen(text) + 3));
	if (!buf) return GF_OUT_OF_MEM;

	strcpy(buf, sdp->sdpText);
	strcat(buf, text);
	strcat(buf, "\r\n");
	gf_free(sdp->sdpText);
	ReorderSDP(buf, GF_FALSE);
	sdp->sdpText = buf;
	return GF_OK;
}