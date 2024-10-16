GF_Err gf_isom_svc_config_update(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, GF_AVCConfig *cfg, Bool is_add)
{
	return gf_isom_avc_config_update_ex(the_file, trackNumber, DescriptionIndex, cfg, is_add ? 1 : 2, GF_FALSE);
}