GF_Box *extr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ExtraDataBox, GF_ISOM_BOX_TYPE_EXTR);
	return (GF_Box *)tmp;
}