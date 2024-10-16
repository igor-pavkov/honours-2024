GF_Box *unkn_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_UnknownBox, GF_ISOM_BOX_TYPE_UNKNOWN);
	return (GF_Box *) tmp;
}