Bool print_version(char *arg_val, u32 param)
{
	fprintf(stderr, "MP4Box - GPAC version %s\n"
	        "%s\n"
	        "GPAC Configuration: " GPAC_CONFIGURATION "\n"
	        "Features: %s %s\n", gf_gpac_version(), gf_gpac_copyright_cite(), gf_sys_features(GF_FALSE), gf_sys_features(GF_TRUE));
	return GF_TRUE;
}