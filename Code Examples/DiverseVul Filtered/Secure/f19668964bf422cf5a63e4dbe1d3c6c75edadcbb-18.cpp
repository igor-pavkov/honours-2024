GF_Box *schi_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SchemeInformationBox, GF_ISOM_BOX_TYPE_SCHI);
	return (GF_Box *)tmp;
}