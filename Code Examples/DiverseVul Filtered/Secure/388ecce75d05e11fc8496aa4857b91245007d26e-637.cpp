GF_Box *ccst_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_CodingConstraintsBox, GF_ISOM_BOX_TYPE_CCST);
	return (GF_Box *) tmp;
}