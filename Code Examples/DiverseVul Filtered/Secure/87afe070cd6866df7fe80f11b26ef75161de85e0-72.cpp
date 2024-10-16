void PrintSWFUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# SWF Importer Options\n"
	        "\n"
	        "MP4Box can import simple Macromedia Flash files (\".SWF\")\n"
	        "You can specify a SWF input file with \'-bt\', \'-xmt\' and \'-mp4\' options\n"
	        "  \n"
	        "Options:\n"
	);
	while (m4b_swf_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_swf_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-extract");
	}
}