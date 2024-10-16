static void PrintSplitUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "  \n"
		"# File splitting\n"
		"MP4Box can split IsoMedia files by size, duration or extract a given part of the file to new IsoMedia file(s).\n"
		"This requires that at most one track in the input file has non random-access points (typically one video track at most).\n"
		"splitting will ignore all MPEG-4 Systems tracks and hint tracks, but will try to split private media tracks.\n"
		"The input file must have enough random access points in order to be split. If this is not the case, you will have to re-encode the content.\n"
		"You can add media to a file and split it in the same pass. In this case, the destination file (the one which would be obtained without splitting) will not be stored.\n"
		"  \n"
	);

	i=0;
	while (m4b_split_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_split_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-split");
	}

}