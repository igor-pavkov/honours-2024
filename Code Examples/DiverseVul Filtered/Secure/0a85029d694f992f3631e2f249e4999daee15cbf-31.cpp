GF_Err gf_isom_mvc_config_del(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	return gf_isom_svc_mvc_config_del(the_file, trackNumber, DescriptionIndex, GF_TRUE);
}