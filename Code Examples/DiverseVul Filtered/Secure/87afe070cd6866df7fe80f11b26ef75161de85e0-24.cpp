u32 parse_dnal(char *arg_val, u32 opt)
{
	dump_nal = atoi(arg_val);
	dump_nal_type = opt;
	return 0;
}