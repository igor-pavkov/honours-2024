void iods_box_del(GF_Box *s)
{
	GF_ObjectDescriptorBox *ptr = (GF_ObjectDescriptorBox *)s;
	if (ptr == NULL) return;
	if (ptr->descriptor) gf_odf_desc_del(ptr->descriptor);
	gf_free(ptr);
}