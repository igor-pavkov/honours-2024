GF_Box *grpi_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_OMADRMGroupIDBox, GF_ISOM_BOX_TYPE_GRPI);
	return (GF_Box *)tmp;
}