void gf_hevc_parse_ps(GF_HEVCConfig* hevccfg, HEVCState* hevc, u32 nal_type)
{
	u32 i, j;
	if (!hevccfg) return;
	
	for (i = 0; i < gf_list_count(hevccfg->param_array); i++) {
		GF_NALUFFParamArray* ar = gf_list_get(hevccfg->param_array, i);
		if (ar->type != nal_type) continue;
		for (j = 0; j < gf_list_count(ar->nalus); j++) {
			u8 ntype, tid, lid;
			GF_NALUFFParam* sl = gf_list_get(ar->nalus, j);
			gf_hevc_parse_nalu(sl->data, sl->size, hevc, &ntype, &tid, &lid);
		}
	}
}