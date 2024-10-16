GF_Err gf_isom_hevc_set_inband_config(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, Bool keep_xps)
{
	return gf_isom_hevc_config_update_ex(the_file, trackNumber, DescriptionIndex, NULL, GF_ISOM_HVCC_SET_INBAND, keep_xps);
}