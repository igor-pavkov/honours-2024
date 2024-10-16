GF_Err gf_isom_avc_config_update(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, GF_AVCConfig *cfg)
{
	return gf_isom_avc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, 0, GF_FALSE);
}