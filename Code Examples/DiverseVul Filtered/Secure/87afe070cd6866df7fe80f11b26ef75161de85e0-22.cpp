u32 parse_dump_ts(char *arg_val, u32 opt)
{
	dump_timestamps = 1;
	if (arg_val) {
		if (isdigit(arg_val[0])) {
			program_number = atoi(arg_val);
		} else {
			return 3;
		}
	}
	return 0;
}