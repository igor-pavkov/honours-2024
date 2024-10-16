void m4ds_box_del(GF_Box *s)
{
	GF_MPEG4ExtensionDescriptorsBox *ptr = (GF_MPEG4ExtensionDescriptorsBox *)s;
	gf_odf_desc_list_del(ptr->descriptors);
	gf_list_del(ptr->descriptors);
	gf_free(ptr);
}