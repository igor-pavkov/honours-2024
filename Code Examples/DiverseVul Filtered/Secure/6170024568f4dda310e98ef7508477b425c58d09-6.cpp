GF_Box *chrm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ChromaInfoBox, GF_QT_BOX_TYPE_CHRM);
	return (GF_Box *)tmp;
}