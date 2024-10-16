GF_Box *kind_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_KindBox, GF_ISOM_BOX_TYPE_KIND);
	return (GF_Box *)tmp;
}