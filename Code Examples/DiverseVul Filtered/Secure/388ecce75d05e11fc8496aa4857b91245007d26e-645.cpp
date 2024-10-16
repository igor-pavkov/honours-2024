GF_Box *dimm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DIMMBox, GF_ISOM_BOX_TYPE_DIMM);
	return (GF_Box *)tmp;
}