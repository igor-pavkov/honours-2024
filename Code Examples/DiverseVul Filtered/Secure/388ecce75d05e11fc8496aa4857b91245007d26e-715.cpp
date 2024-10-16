GF_Box *iods_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ObjectDescriptorBox, GF_ISOM_BOX_TYPE_IODS);
	return (GF_Box *)tmp;
}