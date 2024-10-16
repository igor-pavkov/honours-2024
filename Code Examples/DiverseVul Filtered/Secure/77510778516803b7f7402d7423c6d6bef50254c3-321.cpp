void segr_box_del(GF_Box *s)
{
	u32 i;
	FDSessionGroupBox *ptr = (FDSessionGroupBox *)s;
	if (ptr == NULL) return;
	for (i=0; i<ptr->num_session_groups; i++) {
		if (ptr->session_groups[i].group_ids) gf_free(ptr->session_groups[i].group_ids);
		if (ptr->session_groups[i].channels) gf_free(ptr->session_groups[i].channels);
	}
	if (ptr->session_groups) gf_free(ptr->session_groups);
	gf_free(ptr);
}