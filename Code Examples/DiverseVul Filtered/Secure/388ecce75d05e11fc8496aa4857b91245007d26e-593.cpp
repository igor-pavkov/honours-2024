GF_Box *hmhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_HintMediaHeaderBox, GF_ISOM_BOX_TYPE_HMHD);
	return (GF_Box *)tmp;
}