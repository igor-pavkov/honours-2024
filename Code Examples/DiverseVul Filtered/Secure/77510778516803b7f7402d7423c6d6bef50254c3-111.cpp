GF_Err segr_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i, k;
	FDSessionGroupBox *ptr = (FDSessionGroupBox *)s;

	ISOM_DECREASE_SIZE(ptr, 2);
	ptr->num_session_groups = gf_bs_read_u16(bs);
	if (ptr->size < ptr->num_session_groups) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in segr\n", ptr->num_session_groups));
		ptr->num_session_groups = 0;
		return GF_ISOM_INVALID_FILE;
	}

	GF_SAFE_ALLOC_N(ptr->session_groups, ptr->num_session_groups, SessionGroupEntry);
	if (!ptr->session_groups) return GF_OUT_OF_MEM;

	for (i=0; i<ptr->num_session_groups; i++) {
		ptr->session_groups[i].nb_groups = gf_bs_read_u8(bs);
		ISOM_DECREASE_SIZE(ptr, 1);

		ISOM_DECREASE_SIZE(ptr, ptr->session_groups[i].nb_groups*4);

		GF_SAFE_ALLOC_N(ptr->session_groups[i].group_ids, ptr->session_groups[i].nb_groups, u32);
		if (!ptr->session_groups[i].group_ids) return GF_OUT_OF_MEM;

		for (k=0; k<ptr->session_groups[i].nb_groups; k++) {
			ptr->session_groups[i].group_ids[k] = gf_bs_read_u32(bs);
		}

		ptr->session_groups[i].nb_channels = gf_bs_read_u16(bs);
		ISOM_DECREASE_SIZE(ptr, ptr->session_groups[i].nb_channels*4);

		GF_SAFE_ALLOC_N(ptr->session_groups[i].channels, ptr->session_groups[i].nb_channels, u32);
		if (!ptr->session_groups[i].channels) return GF_OUT_OF_MEM;

		for (k=0; k<ptr->session_groups[i].nb_channels; k++) {
			ptr->session_groups[i].channels[k] = gf_bs_read_u32(bs);
		}
	}
	return GF_OK;
}