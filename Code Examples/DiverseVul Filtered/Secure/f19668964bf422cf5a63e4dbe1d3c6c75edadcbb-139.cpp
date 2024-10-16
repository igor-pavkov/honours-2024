GF_Box *iSFM_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ISMASampleFormatBox, GF_ISOM_BOX_TYPE_ISFM);
	return (GF_Box *)tmp;
}