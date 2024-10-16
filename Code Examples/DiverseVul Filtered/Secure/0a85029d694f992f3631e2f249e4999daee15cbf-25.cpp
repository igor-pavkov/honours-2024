GF_Err av1c_box_size(GF_Box *s) {
	u32 i;
	GF_AV1ConfigurationBox *ptr = (GF_AV1ConfigurationBox *)s;

	if (!ptr->config) {
		ptr->size = 0;
		return GF_BAD_PARAM;
	}

	ptr->size += 4;

	for (i = 0; i < gf_list_count(ptr->config->obu_array); ++i) {
		GF_AV1_OBUArrayEntry *a = gf_list_get(ptr->config->obu_array, i);
		ptr->size += a->obu_length;
	}

	return GF_OK;
}