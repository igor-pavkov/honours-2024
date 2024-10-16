GF_Err hvcc_box_size(GF_Box *s)
{
	u32 i, count, j, subcount;
	GF_HEVCConfigurationBox *ptr = (GF_HEVCConfigurationBox *)s;

	if (!ptr->config) {
		ptr->size = 0;
		return GF_OK;
	}

	if (!ptr->config->is_lhvc)
		ptr->size += 23;
	else
		ptr->size += 6;

	count = gf_list_count(ptr->config->param_array);
	for (i=0; i<count; i++) {
		GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(ptr->config->param_array, i);
		ptr->size += 3;
		subcount = gf_list_count(ar->nalus);
		for (j=0; j<subcount; j++) {
			ptr->size += 2 + ((GF_NALUFFParam *)gf_list_get(ar->nalus, j))->size;
		}
	}
	return GF_OK;
}