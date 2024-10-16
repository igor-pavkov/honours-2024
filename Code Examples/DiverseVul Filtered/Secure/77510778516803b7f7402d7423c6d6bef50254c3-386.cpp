GF_Box *ainf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AssetInformationBox, GF_ISOM_BOX_TYPE_AINF);
	return (GF_Box *)tmp;
}