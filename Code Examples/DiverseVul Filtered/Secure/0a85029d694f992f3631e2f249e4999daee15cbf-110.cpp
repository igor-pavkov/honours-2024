GF_Err m4ds_box_size(GF_Box *s)
{
	GF_Err e;
	u32 descSize = 0;
	GF_MPEG4ExtensionDescriptorsBox *ptr = (GF_MPEG4ExtensionDescriptorsBox *)s;
	e = gf_odf_desc_list_size(ptr->descriptors, &descSize);
	ptr->size += descSize;
	return e;
}