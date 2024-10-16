static void nalu_merge_ps(GF_BitStream *ps_bs, Bool rewrite_start_codes, u32 nal_unit_size_field, GF_MPEGVisualSampleEntryBox *entry, Bool is_hevc, Bool *has_vps)
{
	u32 i, count;
	if (is_hevc) {
		if (entry->hevc_config) {
			count = gf_list_count(entry->hevc_config->config->param_array);
			for (i=0; i<count; i++) {
				GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(entry->hevc_config->config->param_array, i);
				if (ar->type == GF_HEVC_NALU_VID_PARAM) {
					if (! *has_vps)  *has_vps = GF_TRUE;
					else continue;
				}
				rewrite_nalus_list(ar->nalus, ps_bs, rewrite_start_codes, nal_unit_size_field);
			}
		}
		if (entry->lhvc_config) {
			count = gf_list_count(entry->lhvc_config->config->param_array);
			for (i=0; i<count; i++) {
				GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(entry->lhvc_config->config->param_array, i);
				if (ar->type == GF_HEVC_NALU_VID_PARAM) {
					if (! *has_vps)  *has_vps = GF_TRUE;
					else continue;
				}
				rewrite_nalus_list(ar->nalus, ps_bs, rewrite_start_codes, nal_unit_size_field);
			}
		}
	} else {
		if (entry->avc_config) {
			rewrite_nalus_list(entry->avc_config->config->sequenceParameterSets, ps_bs, rewrite_start_codes, nal_unit_size_field);
			rewrite_nalus_list(entry->avc_config->config->sequenceParameterSetExtensions, ps_bs, rewrite_start_codes, nal_unit_size_field);
			rewrite_nalus_list(entry->avc_config->config->pictureParameterSets, ps_bs, rewrite_start_codes, nal_unit_size_field);
		}

		/*add svc config */
		if (entry->svc_config) {
			rewrite_nalus_list(entry->svc_config->config->sequenceParameterSets, ps_bs, rewrite_start_codes, nal_unit_size_field);
			rewrite_nalus_list(entry->svc_config->config->pictureParameterSets, ps_bs, rewrite_start_codes, nal_unit_size_field);
		}
		/*add mvc config */
		if (entry->mvc_config) {
			rewrite_nalus_list(entry->mvc_config->config->sequenceParameterSets, ps_bs, rewrite_start_codes, nal_unit_size_field);
			rewrite_nalus_list(entry->mvc_config->config->pictureParameterSets, ps_bs, rewrite_start_codes, nal_unit_size_field);
		}
	}
}