GF_Box *name_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_NameBox, GF_ISOM_BOX_TYPE_NAME);
	return (GF_Box *)tmp;
}