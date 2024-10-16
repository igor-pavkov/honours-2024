void PrintExtractUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# Extracting Options\n"
	"MP4Box can be used to extract media tracks from MP4 files. If you need to convert these tracks however, please check the [filters doc](Filters).\n"
	"  \n"
	"Options:\n"
	);
	while (m4b_extr_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_extr_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-extract");
	}
}