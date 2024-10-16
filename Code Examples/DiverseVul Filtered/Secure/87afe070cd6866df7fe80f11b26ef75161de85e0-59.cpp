u32 parse_dashlive(char *arg, char *arg_val, u32 opt)
{
	dash_mode = opt ? GF_DASH_DYNAMIC_DEBUG : GF_DASH_DYNAMIC;
	dash_live = 1;
	if (arg[10] == '=') {
		dash_ctx_file = arg + 11;
	}
	dash_duration = atof(arg_val);
	return 0;
}