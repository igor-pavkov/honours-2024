void stri_box_del(GF_Box *s)
{
	GF_SubTrackInformationBox *ptr = (GF_SubTrackInformationBox *)s;
	if (ptr == NULL) return;
	if (ptr->attribute_list) gf_free(ptr->attribute_list);
	gf_free(ptr);
}