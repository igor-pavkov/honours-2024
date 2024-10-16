u32 parse_ttxt(char *arg_val, u32 opt)
{
	if (opt) //-srt
		dump_srt = GF_TRUE;
	else
		dump_ttxt = GF_TRUE;

	import_subtitle = 1;
	trackID = 0;

	if (arg_val && (!strcmp(arg_val, "*") || !strcmp(arg_val, "@") || !strcmp(arg_val, "all")) ) {
		trackID = (u32)-1;
	} else if (arg_val) {
		if (sscanf(arg_val, "%u", &trackID) == 1) {
			char szTk[20];
			sprintf(szTk, "%d", trackID);
			if (strcmp(szTk, arg_val))
				trackID = 0;
		}
		if (!trackID) return 3;
	}
	return 0;
}