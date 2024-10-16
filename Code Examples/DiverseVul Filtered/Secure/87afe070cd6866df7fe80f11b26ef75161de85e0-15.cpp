u32 parse_dsap(char *arg_val, u32 opt)
{
	dump_saps = atoi(arg_val);
	dump_saps_mode = opt;
	return 0;
}