GF_Box *mfra_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieFragmentRandomAccessBox, GF_ISOM_BOX_TYPE_MFRA);
	tmp->tfra_list = gf_list_new();
	return (GF_Box *)tmp;
}