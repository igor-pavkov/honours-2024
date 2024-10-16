u32 parse_sdtp(char *arg_val, u32 opt)
{
	if (!stricmp(arg_val, "both")) sdtp_in_traf = 2;
	else if (!stricmp(arg_val, "sdtp")) sdtp_in_traf = 1;
	else sdtp_in_traf = 0;
	return 0;
}