u32 parse_dash_profile(char *arg_val, u32 opt)
{
	if (!stricmp(arg_val, "live") || !stricmp(arg_val, "simple")) dash_profile = GF_DASH_PROFILE_LIVE;
	else if (!stricmp(arg_val, "onDemand")) dash_profile = GF_DASH_PROFILE_ONDEMAND;
	else if (!stricmp(arg_val, "hbbtv1.5:live") || !stricmp(arg_val, "hbbtv1.5.live"))
		dash_profile = GF_DASH_PROFILE_HBBTV_1_5_ISOBMF_LIVE;
	else if (!stricmp(arg_val, "dashavc264:live") || !stricmp(arg_val, "dashavc264.live"))
		dash_profile = GF_DASH_PROFILE_AVC264_LIVE;
	else if (!stricmp(arg_val, "dashavc264:onDemand") || !stricmp(arg_val, "dashavc264.onDemand"))
		dash_profile = GF_DASH_PROFILE_AVC264_ONDEMAND;
	else if (!stricmp(arg_val, "dashif.ll")) dash_profile = GF_DASH_PROFILE_DASHIF_LL;
	else if (!stricmp(arg_val, "main")) dash_profile = GF_DASH_PROFILE_MAIN;
	else if (!stricmp(arg_val, "full")) dash_profile = GF_DASH_PROFILE_FULL;
	else {
		M4_LOG(GF_LOG_ERROR, ("Unrecognized DASH profile \"%s\" - please check usage\n", arg_val));
		return 2;
	}
	return 0;
}