GF_Err gf_isom_vvc_set_inband_config(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, Bool keep_xps)
{
	return gf_isom_vvc_config_update_ex(the_file, trackNumber, DescriptionIndex, NULL, GF_ISOM_VVCC_SET_INBAND, keep_xps);
}