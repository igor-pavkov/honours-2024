GF_Err hnti_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_HintTrackInfoBox *hnti = (GF_HintTrackInfoBox *)s;
	if (!hnti || !a) return GF_BAD_PARAM;

	switch (a->type) {
	//this is the value for GF_RTPBox - same as HintSampleEntry for RTP !!!
	case GF_ISOM_BOX_TYPE_RTP:
	case GF_ISOM_BOX_TYPE_SDP:
		if (hnti->SDP) ERROR_ON_DUPLICATED_BOX(a, s)
		hnti->SDP = a;
		break;
	default:
		break;
	}
	return GF_OK;
}