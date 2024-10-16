void PrintLiveUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# Live Scene Encoder Options\n"
	        "The options shall be specified as òpt_name=opt_val.\n"
	        "Options:\n"
	        "\n"
	);
	while (m4b_liveenc_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_liveenc_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-extract");
	}

	gf_sys_format_help(helpout, help_flags, "  \n"
		"Runtime options:\n"
		"- q: quits\n"
		"- u: inputs some commands to be sent\n"
		"- U: same as u but signals the updates as critical\n"
		"- e: inputs some commands to be sent without being aggregated\n"
		"- E: same as e but signals the updates as critical\n"
		"- f: forces RAP sending\n"
		"- F: forces RAP regeneration and sending\n"
		"- p: dumps current scene\n"
	);
}