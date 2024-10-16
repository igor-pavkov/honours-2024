GF_Box *mdri_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_OMADRMMutableInformationBox, GF_ISOM_BOX_TYPE_MDRI);
	return (GF_Box *)tmp;
}