GF_Err gf_isom_hevc_set_tile_config(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, GF_HEVCConfig *cfg, Bool is_base_track)
{
	return gf_isom_hevc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, is_base_track ? GF_ISOM_HVCC_SET_TILE_BASE_TRACK : GF_ISOM_HVCC_SET_TILE, GF_FALSE);
}