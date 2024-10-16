GF_Box *mehd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieExtendsHeaderBox, GF_ISOM_BOX_TYPE_MEHD);
	return (GF_Box *)tmp;
}