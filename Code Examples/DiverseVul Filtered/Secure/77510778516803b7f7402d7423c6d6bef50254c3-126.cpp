GF_Box *sdtp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleDependencyTypeBox, GF_ISOM_BOX_TYPE_SDTP);
	return (GF_Box *)tmp;
}