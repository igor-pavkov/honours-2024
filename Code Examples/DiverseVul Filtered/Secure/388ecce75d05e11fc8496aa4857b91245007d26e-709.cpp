GF_Box *minf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MediaInformationBox, GF_ISOM_BOX_TYPE_MINF);
	return (GF_Box *)tmp;
}