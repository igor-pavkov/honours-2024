void PrintUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "MP4Box [option] input [option]\n"
		"  \n"
		"# General Options:\n"
	);
	while (m4b_usage_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_usage_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-general");
	}
}