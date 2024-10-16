void PrintDumpUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# File Dumping\n"
	"  \n"
	"MP4Box has many dump functionalities, from simple track listing to more complete reporting of special tracks.\n"
	"  \n"
	"Options:\n"
	);
	while (m4b_dump_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_dump_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-extract");
	}
}