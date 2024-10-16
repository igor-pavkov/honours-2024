GF_Err gf_isom_lhvc_config_update(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, GF_HEVCConfig *cfg, GF_ISOMLHEVCTrackType track_type)
{
	if (cfg) cfg->is_lhvc = GF_TRUE;
	switch (track_type) {
	case GF_ISOM_LEHVC_ONLY:
		return gf_isom_hevc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, GF_ISOM_HVCC_SET_LHVC, GF_FALSE);
	case GF_ISOM_LEHVC_WITH_BASE:
		return gf_isom_hevc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, GF_ISOM_HVCC_SET_LHVC_WITH_BASE, GF_FALSE);
	case GF_ISOM_LEHVC_WITH_BASE_BACKWARD:
		return gf_isom_hevc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, GF_ISOM_HVCC_SET_LHVC_WITH_BASE_BACKWARD, GF_FALSE);
	case GF_ISOM_HEVC_TILE_BASE:
		return gf_isom_hevc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, GF_ISOM_HVCC_SET_HEVC_TILE_BASE, GF_FALSE);
	default:
		return GF_BAD_PARAM;
	}
}