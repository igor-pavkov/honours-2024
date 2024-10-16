GF_Box *hnti_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_HintTrackInfoBox, GF_ISOM_BOX_TYPE_HNTI);
	return (GF_Box *)tmp;
}