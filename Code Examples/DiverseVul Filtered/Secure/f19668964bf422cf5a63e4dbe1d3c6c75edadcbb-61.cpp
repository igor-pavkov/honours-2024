void grpi_box_del(GF_Box *s)
{
	GF_OMADRMGroupIDBox *ptr = (GF_OMADRMGroupIDBox *)s;
	if (ptr == NULL) return;
	if (ptr->GroupID) gf_free(ptr->GroupID);
	if (ptr->GroupKey) gf_free(ptr->GroupKey);
	gf_free(ptr);
}