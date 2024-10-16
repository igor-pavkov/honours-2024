void PrintGeneralUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# General Options\n"
		"MP4Box is a multimedia packager, with a vast number of functionalities: conversion, splitting, hinting, dumping, DASH-ing, encryption, transcoding and others.\n"
		"MP4Box provides a large set of options, classified by categories (see [-h]()). These options do not follow any particular ordering.\n"
		"MP4Box performs in-place rewrite of IsoMedia files (the input file is overwritten). You can change this behavior by using the [-out]() option.\n"
		"MP4Box stores by default the file with 0.5 second interleaving and meta-data (`moov` ...) at the beginning, making it suitable for HTTP streaming. This may however takes longer to store the file, use [-flat]() to change this behavior.\n"
		"MP4Box usually generates a temporary file when creating a new IsoMedia file. The location of this temporary file is OS-dependent, and it may happen that the drive/partition the temporary file is created on has not enough space or no write access. In such a case, you can specify a temporary file location with [-tmp]().\n"
		"Note: Track operations identify tracks through their ID (usually referred to as tkID in the help), not their order.\n"
		"Option values:\n"
		"Unless specified otherwise, an option of type `integer` expects a trackID value following it."
		"An option of type `boolean` expects no following value."
		"  \n"
	);


	while (m4b_gen_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_gen_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-gen");
	}
}