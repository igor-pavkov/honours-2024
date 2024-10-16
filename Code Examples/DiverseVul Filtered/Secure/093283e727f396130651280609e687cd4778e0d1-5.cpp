void afra_box_del(GF_Box *s)
{
	GF_AdobeFragRandomAccessBox *ptr = (GF_AdobeFragRandomAccessBox *)s;
	if (ptr == NULL) return;

	while (gf_list_count(ptr->local_access_entries)) {
		gf_free(gf_list_get(ptr->local_access_entries, 0));
		gf_list_rem(ptr->local_access_entries, 0);
	}
	gf_list_del(ptr->local_access_entries);

	while (gf_list_count(ptr->global_access_entries)) {
		gf_free(gf_list_get(ptr->global_access_entries, 0));
		gf_list_rem(ptr->global_access_entries, 0);
	}
	gf_list_del(ptr->global_access_entries);

	gf_free(ptr);
}