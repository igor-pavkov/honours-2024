GF_Box *jp2h_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_J2KHeaderBox, GF_ISOM_BOX_TYPE_JP2H);
	return (GF_Box *)tmp;
}