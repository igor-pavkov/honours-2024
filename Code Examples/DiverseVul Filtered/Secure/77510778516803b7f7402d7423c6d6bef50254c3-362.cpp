GF_Box *rvcc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_RVCConfigurationBox, GF_ISOM_BOX_TYPE_RVCC);
	return (GF_Box *)tmp;
}