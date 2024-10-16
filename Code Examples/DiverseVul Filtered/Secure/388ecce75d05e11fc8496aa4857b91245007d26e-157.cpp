void url_box_del(GF_Box *s)
{
	GF_DataEntryURLBox *ptr = (GF_DataEntryURLBox *)s;
	if (ptr == NULL) return;
	if (ptr->location) gf_free(ptr->location);
	gf_free(ptr);
	return;
}