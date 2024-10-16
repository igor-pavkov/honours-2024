GF_Err segr_box_size(GF_Box *s)
{
	u32 i;
	FDSessionGroupBox *ptr = (FDSessionGroupBox *)s;

	ptr->size += 2;

	for (i=0; i<ptr->num_session_groups; i++) {
		ptr->size += 1 + 4*ptr->session_groups[i].nb_groups;
		ptr->size += 2 + 4*ptr->session_groups[i].nb_channels;
	}
	return GF_OK;
}