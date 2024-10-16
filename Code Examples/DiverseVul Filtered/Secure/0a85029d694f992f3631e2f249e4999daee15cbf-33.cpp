static GF_VPConfig* VP_DuplicateConfig(GF_VPConfig const * const cfg)
{
	GF_VPConfig *out = gf_odf_vp_cfg_new();
	if (out) {
		out->profile = cfg->profile;
		out->level = cfg->level;
		out->bit_depth = cfg->bit_depth;
		out->chroma_subsampling = cfg->chroma_subsampling;
		out->video_fullRange_flag = cfg->video_fullRange_flag;
		out->colour_primaries = cfg->colour_primaries;
		out->transfer_characteristics = cfg->transfer_characteristics;
		out->matrix_coefficients = cfg->matrix_coefficients;
	}

	return out;
}