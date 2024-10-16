GF_Err gf_isom_avc_set_inband_config(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, Bool keep_xps)
{
	return gf_isom_avc_config_update_ex(the_file, trackNumber, DescriptionIndex, NULL, 3, keep_xps);
}