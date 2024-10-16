u32 parse_help(char *arg_val, u32 opt)
{
	if (!arg_val) PrintUsage();
	else if (opt) PrintHelp(arg_val, GF_TRUE, GF_FALSE);
	else if (!strcmp(arg_val, "general")) PrintGeneralUsage();
	else if (!strcmp(arg_val, "extract")) PrintExtractUsage();
	else if (!strcmp(arg_val, "split")) PrintSplitUsage();
	else if (!strcmp(arg_val, "dash")) PrintDASHUsage();
	else if (!strcmp(arg_val, "dump")) PrintDumpUsage();
	else if (!strcmp(arg_val, "import")) PrintImportUsage();
	else if (!strcmp(arg_val, "format")) {
		M4_LOG(GF_LOG_WARNING, ("deprecated, see [filters documentation](Filters)\n"));
	}
	else if (!strcmp(arg_val, "hint")) PrintHintUsage();
	else if (!strcmp(arg_val, "encode")) PrintEncodeUsage();
	else if (!strcmp(arg_val, "crypt")) PrintEncryptUsage();
	else if (!strcmp(arg_val, "meta")) PrintMetaUsage();
	else if (!strcmp(arg_val, "swf")) PrintSWFUsage();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
	else if (!strcmp(arg_val, "rtp")) {
		M4_LOG(GF_LOG_WARNING, ("RTP streaming deprecated in MP4Box, use gpac application\n"));
	}
	else if (!strcmp(arg_val, "live")) PrintLiveUsage();
#endif
	else if (!strcmp(arg_val, "core")) PrintCoreUsage();
	else if (!strcmp(arg_val, "tags")) PrintTags();
	else if (!strcmp(arg_val, "cicp")) PrintCICP();
	else if (!strcmp(arg_val, "all")) {
		PrintGeneralUsage();
		PrintExtractUsage();
		PrintDASHUsage();
		PrintSplitUsage();
		PrintDumpUsage();
		PrintImportUsage();
		PrintHintUsage();
		PrintEncodeUsage();
		PrintEncryptUsage();
		PrintMetaUsage();
		PrintSWFUsage();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
		PrintLiveUsage();
#endif
		PrintCoreUsage();
		PrintTags();
		PrintCICP();
	} else if (!strcmp(arg_val, "opts")) {
		PrintHelp("@", GF_FALSE, GF_FALSE);
	} else {
		PrintHelp(arg_val, GF_FALSE, GF_FALSE);
	}
	return 1;
}