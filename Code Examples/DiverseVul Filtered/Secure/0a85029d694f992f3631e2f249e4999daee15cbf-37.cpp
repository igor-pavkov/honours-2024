GF_Err gf_isom_lhvc_force_inband_config(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	return gf_isom_hevc_config_update_ex(the_file, trackNumber, DescriptionIndex, NULL, GF_ISOM_LHCC_SET_INBAND, GF_FALSE);
}