static void merge_avc_config(GF_AVCConfig *dst_cfg, GF_AVCConfig *src_cfg)
{
	GF_AVCConfig *cfg;
	if (!src_cfg || !dst_cfg) return;
	cfg = AVC_DuplicateConfig(src_cfg);
	if (!cfg) return;
	
	while (gf_list_count(cfg->sequenceParameterSets)) {
		GF_NALUFFParam *p = (GF_NALUFFParam*)gf_list_get(cfg->sequenceParameterSets, 0);
		gf_list_rem(cfg->sequenceParameterSets, 0);
		gf_list_insert(dst_cfg->sequenceParameterSets, p, 0);
	}
	while (gf_list_count(cfg->pictureParameterSets)) {
		GF_NALUFFParam *p = (GF_NALUFFParam*)gf_list_get(cfg->pictureParameterSets, 0);
		gf_list_rem(cfg->pictureParameterSets, 0);
		gf_list_insert(dst_cfg->pictureParameterSets, p, 0);
	}
	gf_odf_avc_cfg_del(cfg);
}