u32 parse_multi_rtp(char *arg_val, u32 opt)
{
	hint_flags |= GP_RTP_PCK_USE_MULTI;
	if (arg_val)
		max_ptime = atoi(arg_val);
	return 0;
}