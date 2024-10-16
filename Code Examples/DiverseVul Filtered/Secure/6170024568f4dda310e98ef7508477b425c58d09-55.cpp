void alis_box_del(GF_Box *s)
{
	GF_DataEntryAliasBox *ptr = (GF_DataEntryAliasBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}