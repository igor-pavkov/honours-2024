GF_Box *saiz_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleAuxiliaryInfoSizeBox, GF_ISOM_BOX_TYPE_SAIZ);
	return (GF_Box *)tmp;
}