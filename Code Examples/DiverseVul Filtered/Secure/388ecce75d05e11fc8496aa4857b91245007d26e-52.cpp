void esds_box_del(GF_Box *s)
{
	GF_ESDBox *ptr = (GF_ESDBox *)s;
	if (ptr == NULL)	return;
	if (ptr->desc) gf_odf_desc_del((GF_Descriptor *)ptr->desc);
	gf_free(ptr);
}