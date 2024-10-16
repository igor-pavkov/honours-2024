u32 parse_bs_switch(char *arg_val, u32 opt)
{
	if (!stricmp(arg_val, "no") || !stricmp(arg_val, "off")) bitstream_switching_mode = GF_DASH_BSMODE_NONE;
	else if (!stricmp(arg_val, "merge"))  bitstream_switching_mode = GF_DASH_BSMODE_MERGED;
	else if (!stricmp(arg_val, "multi"))  bitstream_switching_mode = GF_DASH_BSMODE_MULTIPLE_ENTRIES;
	else if (!stricmp(arg_val, "single"))  bitstream_switching_mode = GF_DASH_BSMODE_SINGLE;
	else if (!stricmp(arg_val, "inband"))  bitstream_switching_mode = GF_DASH_BSMODE_INBAND;
	else {
		M4_LOG(GF_LOG_ERROR, ("Unrecognized bitstream switching mode \"%s\" - please check usage\n", arg_val));
		return 2;
	}
	return 0;
}