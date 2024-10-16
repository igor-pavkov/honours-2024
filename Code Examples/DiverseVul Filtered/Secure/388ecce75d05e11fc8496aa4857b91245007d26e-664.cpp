GF_Box *mdia_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MediaBox, GF_ISOM_BOX_TYPE_MDIA);
	return (GF_Box *)tmp;
}