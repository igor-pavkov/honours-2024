GF_Box *mdat_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MediaDataBox, GF_ISOM_BOX_TYPE_MDAT);
	return (GF_Box *)tmp;
}