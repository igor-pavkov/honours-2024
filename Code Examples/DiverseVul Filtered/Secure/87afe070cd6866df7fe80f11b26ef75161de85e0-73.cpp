void PrintImportUsage()
{
	u32 i;

	gf_sys_format_help(helpout, help_flags, "# Importing Options\n"
		"# File importing\n"
		"Syntax is [-add]() / [-cat]() `URL[#FRAGMENT][:opt1...:optN=val]`\n"
		"This process will create the destination file if not existing, and add the track(s) to it. If you wish to always create a new destination file, add [-new](MP4B_GEN).\n"
		"The supported input media types depend on your installation, check [filters documentation](Filters) for more info.\n"
		"  \n"
		"To select a desired media track from a source, a fragment identifier '#' can be specified, before any other options. The following syntax is used:\n"
		"- `#video`: adds the first video track found in source\n"
		"- `#audio`: adds the first audio track found in source\n"
		"- `#auxv`: adds the first auxiliary video track found in source\n"
		"- `#pict`: adds the first picture track found in source\n"
		"- `#trackID=ID` or `#ID`: adds the specified track. For IsoMedia files, ID is the track ID. For other media files, ID is the value indicated by `MP4Box -info inputFile`\n"
		"- `#pid=ID`: number of desired PID for MPEG-2 TS sources\n"
		"- `#prog_id=ID`: number of desired program for MPEG-2 TS sources\n"
		"- `#program=NAME`: name of desired program for MPEG-2 TS sources\n"
		"  \n"
		"By default all imports are performed sequentially, and final interleaving is done at the end; this however requires a temporary file holding original ISOBMF file (if any) and added files before creating the final output. Since this can become quite large, it is possible to add media to a new file without temporary storage, using [-flat](MP4B_GEN) option, but this disables media interleaving.\n"
		"  \n"
		"If you wish to create an interleaved new file with no temporary storage, use the [-newfs](MP4B_GEN) option. The interleaving might not be as precise as when using [-new]() since it is dependent on muxer input scheduling (each execution might lead to a slightly different result). Additionally in this mode: \n"
		" - Some muxing options (marked with `X` below) will be activated for all inputs (e.g it is not possible to import one AVC track with `xps_inband` and another without).\n"
		" - Some muxing options (marked as `D` below) cannot be used as they require temporary storage for file edition.\n"
		" - Usage of [-cat]() is possible, but concatenated sources will not be interleaved in the output. If you wish to perform more complex cat/add operations without temp file, use a [playlist](flist).\n"
		"  \n"
		"Source URL can be any URL supported by GPAC, not limited to local files.\n"
		"  \n"
		"Note: When importing SRT or SUB files, MP4Box will choose default layout options to make the subtitle appear at the bottom of the video. You SHOULD NOT import such files before any video track is added to the destination file, otherwise the results will likely not be useful (default SRT/SUB importing uses default serif font, fontSize 18 and display size 400x60). For more details, check [TTXT doc](Subtitling-with-GPAC).\n"
		"  \n"
		"When importing several tracks/sources in one pass, all options will be applied if relevant to each source. These options are set for all imported streams. If you need to specify these options per stream, set per-file options using the syntax `-add stream[:opt1:...:optN]`.\n"
		"  \n"
		"The import file name may be set to empty or `self`, indicating that the import options should be applied to the destination file track(s).\n"
		"EX -add self:moovts=-1:noedit src.mp4\n"
		"This will apply `moovts` and `noedit` option to all tracks in src.mp4\n"
		"EX -add self#2:moovts=-1:noedit src.mp4\n"
		"This will apply `moovts` and `noedit` option to track with `ID=2` in src.mp4\n"
		"Only per-file options marked with a `S` are possible in this mode.\n"
		"  \n"
		"When importing an ISOBMFF/QT file, only options marked as `C` or `S` can be used.\n"
		"  \n"
		"Allowed per-file options:\n\n"
	);

	i=0;
	while (m4b_imp_fileopt_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_imp_fileopt_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags | GF_PRINTARG_NO_DASH, arg, "mp4box-import");
	}

	gf_sys_format_help(helpout, help_flags, "\n"
		"Note: `sopt`, `dopt` and `@f` must be placed after all other options.\n"
		"# Global import options\n"
	);

	i=0;
	while (m4b_imp_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_imp_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-import");
	}
}