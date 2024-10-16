static Bool nalu_cleanup_config(GF_List *param_array, Bool set_inband, Bool keep_xps)
{
	u32 i;
	Bool array_incomplete = set_inband;
	if (!param_array) return 0;

	for (i=0; i<gf_list_count(param_array); i++) {
		GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(param_array, i);

		/*we want to force inband signaling*/
		if (set_inband) {
			ar->array_completeness = 0;
			if (keep_xps) {
				array_incomplete=1;
				continue;
			}

			while (gf_list_count(ar->nalus)) {
				GF_NALUFFParam *sl = (GF_NALUFFParam*)gf_list_get(ar->nalus, 0);
				gf_list_rem(ar->nalus, 0);
				if (sl->data) gf_free(sl->data);
				gf_free(sl);
			}
			gf_list_del(ar->nalus);
			gf_free(ar);
			ar=NULL;
			gf_list_rem(param_array, i);
			i--;
			continue;
		}
		if (ar && !ar->array_completeness)
			array_incomplete = 1;
	}
	return array_incomplete;
}