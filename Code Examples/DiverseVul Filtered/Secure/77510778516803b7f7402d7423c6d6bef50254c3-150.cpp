void fpar_box_del(GF_Box *s)
{
	FilePartitionBox *ptr = (FilePartitionBox *)s;
	if (ptr == NULL) return;
	if (ptr->scheme_specific_info) gf_free(ptr->scheme_specific_info);
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}