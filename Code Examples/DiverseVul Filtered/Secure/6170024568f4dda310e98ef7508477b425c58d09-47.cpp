void tcmi_box_del(GF_Box *s)
{
	GF_TimeCodeMediaInformationBox *ptr = (GF_TimeCodeMediaInformationBox *)s;
	if (ptr->font) gf_free(ptr->font);
	gf_free(s);
}