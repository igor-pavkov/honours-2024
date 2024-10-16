static void naludmx_add_param_nalu(GF_List *param_list, GF_NALUFFParam *sl, u8 nal_type)
{
	GF_NALUFFParamArray *pa = NULL;
	u32 i, count;
	count = gf_list_count(param_list);
	for (i=0; i<count; i++) {
		pa = gf_list_get(param_list, i);
		if (pa->type == nal_type) break;
		pa = NULL;
	}
	if (!pa) {
		GF_SAFEALLOC(pa, GF_NALUFFParamArray);
		if (!pa) return;

		pa->array_completeness = 1;
		pa->type = nal_type;
		pa->nalus = gf_list_new();
		gf_list_add(param_list, pa);
	}
	gf_list_add(pa->nalus, sl);
}