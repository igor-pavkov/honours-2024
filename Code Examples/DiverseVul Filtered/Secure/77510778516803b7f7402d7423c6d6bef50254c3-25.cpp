void fiin_box_del(GF_Box *s)
{
	FDItemInformationBox *ptr = (FDItemInformationBox *)s;
	if (ptr == NULL) return;
	if (ptr->partition_entries) gf_list_del(ptr->partition_entries);
	gf_free(ptr);
}