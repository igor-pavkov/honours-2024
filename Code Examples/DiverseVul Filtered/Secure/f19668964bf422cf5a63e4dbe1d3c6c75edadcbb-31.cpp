void mdri_box_del(GF_Box *s)
{
	GF_OMADRMMutableInformationBox*ptr = (GF_OMADRMMutableInformationBox*)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}