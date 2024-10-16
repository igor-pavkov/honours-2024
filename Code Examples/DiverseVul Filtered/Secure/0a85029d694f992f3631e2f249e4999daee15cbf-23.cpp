void merge_hevc_config(GF_HEVCConfig *dst_cfg, GF_HEVCConfig *src_cfg, Bool force_insert)
{
	GF_HEVCConfig *cfg = HEVC_DuplicateConfig(src_cfg);
	//merge all xPS
	u32 i, j, count = cfg->param_array ? gf_list_count(cfg->param_array) : 0;
	for (i=0; i<count; i++) {
		GF_NALUFFParamArray *ar_h = NULL;
		u32 count2 = dst_cfg->param_array ? gf_list_count(dst_cfg->param_array) : 0;
		GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(cfg->param_array, i);
		for (j=0; j<count2; j++) {
			ar_h = (GF_NALUFFParamArray*)gf_list_get(dst_cfg->param_array, j);
			if (ar_h->type==ar->type) {
				break;
			}
			ar_h = NULL;
		}
		if (!ar_h) {
			gf_list_add(dst_cfg->param_array, ar);
			gf_list_rem(cfg->param_array, i);
			count--;
			i--;
		} else {
			while (gf_list_count(ar->nalus)) {
				GF_NALUFFParam *p = (GF_NALUFFParam*)gf_list_get(ar->nalus, 0);
				gf_list_rem(ar->nalus, 0);
				if (force_insert)
					gf_list_insert(ar_h->nalus, p, 0);
				else
					gf_list_add(ar_h->nalus, p);
			}

		}
	}
	gf_odf_hevc_cfg_del(cfg);

#define CHECK_CODE(__code)	if (dst_cfg->__code < src_cfg->__code) dst_cfg->__code = src_cfg->__code;

	CHECK_CODE(configurationVersion)
	CHECK_CODE(profile_idc)
	CHECK_CODE(profile_space)
	CHECK_CODE(tier_flag)
	CHECK_CODE(general_profile_compatibility_flags)
	CHECK_CODE(progressive_source_flag)
	CHECK_CODE(interlaced_source_flag)
	CHECK_CODE(constraint_indicator_flags)
	CHECK_CODE(level_idc)
	CHECK_CODE(min_spatial_segmentation_idc)

}