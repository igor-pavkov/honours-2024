GF_Box *bloc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_BaseLocationBox, GF_ISOM_BOX_TYPE_BLOC);
	return (GF_Box *)tmp;
}