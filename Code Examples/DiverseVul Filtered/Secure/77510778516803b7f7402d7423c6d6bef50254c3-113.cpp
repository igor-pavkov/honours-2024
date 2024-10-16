GF_Box *ihdr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_J2KImageHeaderBox, GF_ISOM_BOX_TYPE_IHDR);
	return (GF_Box *)tmp;
}