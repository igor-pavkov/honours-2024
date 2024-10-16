u32 parse_file_info(char *arg_val, u32 opt)
{
	print_info = opt ? 2 : 1;
	if (arg_val) {
		if (sscanf(arg_val, "%u", &info_track_id) == 1) {
			char szTk[20];
			sprintf(szTk, "%u", info_track_id);
			if (strcmp(szTk, arg_val)) info_track_id = 0;
		}
		if (!info_track_id) return 3;
	}
	return 0;
}