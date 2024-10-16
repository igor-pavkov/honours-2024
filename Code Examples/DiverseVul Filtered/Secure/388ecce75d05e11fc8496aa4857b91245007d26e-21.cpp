GF_Box *dinf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DataInformationBox, GF_ISOM_BOX_TYPE_DINF);
	return (GF_Box *)tmp;
}