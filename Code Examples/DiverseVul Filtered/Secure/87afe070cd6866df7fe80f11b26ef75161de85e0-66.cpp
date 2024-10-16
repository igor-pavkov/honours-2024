void PrintMetaUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# Meta and HEIF Options\n"
	"IsoMedia files can be used as generic meta-data containers, for examples storing XML information and sample images for a movie. The resulting file may not always contain a movie as is the case with some HEIF files or MPEG-21 files.\n"
	"  \n"
	"These information can be stored at the file root level, as is the case for HEIF/IFF and MPEG-21 file formats, or at the movie or track level for a regular movie."
	"  \n  \n");
	while (m4b_meta_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_meta_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-extract");
	}
}