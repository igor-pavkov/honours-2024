u32 parse_fps(char *arg_val, u32 opt)
{
	u32 ticks, dts_inc;
	if (!strcmp(arg_val, "auto")) {
		M4_LOG(GF_LOG_WARNING, ("Warning, fps=auto option is deprecated\n"));
	}
	else if ((sscanf(arg_val, "%u-%u", &ticks, &dts_inc)==2) || (sscanf(arg_val, "%u/%u", &ticks, &dts_inc)==2) ) {
		if (!dts_inc) dts_inc = 1;
		import_fps.num = ticks;
		import_fps.den = dts_inc;
	} else {
		import_fps.num = (s32) (1000 * atof(arg_val));
		import_fps.den = 1000;
	}
	return 0;
}