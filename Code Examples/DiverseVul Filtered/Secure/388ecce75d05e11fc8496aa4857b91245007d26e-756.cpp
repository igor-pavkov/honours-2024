GF_Box *esds_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ESDBox, GF_ISOM_BOX_TYPE_ESDS);
	return (GF_Box *)tmp;
}