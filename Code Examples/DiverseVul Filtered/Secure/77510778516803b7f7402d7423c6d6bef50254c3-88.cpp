GF_Box *mvcg_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MultiviewGroupBox, GF_ISOM_BOX_TYPE_MVCG);
	return (GF_Box *)tmp;
}