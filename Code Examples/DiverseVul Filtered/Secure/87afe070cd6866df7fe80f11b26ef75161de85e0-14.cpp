u32 parse_gendoc(char *name, u32 opt)
{
	u32 i=0;
	//gen MD
	if (!opt) {
		help_flags = GF_PRINTARG_MD | GF_PRINTARG_IS_APP;
		helpout = gf_fopen("mp4box-gen-opts.md", "w");

		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » General");
		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
		fprintf(helpout, "# Syntax\n");
		gf_sys_format_help(helpout, help_flags, "MP4Box [option] input [option] [other_dash_inputs]\n"
			"  \n"
		);
		PrintGeneralUsage();
		PrintEncryptUsage();
		fprintf(helpout, "# Help Options\n");
		while (m4b_usage_args[i].name) {
			GF_GPACArg *g_arg = (GF_GPACArg *) &m4b_usage_args[i];
			i++;
			gf_sys_print_arg(helpout, help_flags, g_arg, "mp4box-general");
		}

		gf_fclose(helpout);

		helpout = gf_fopen("mp4box-import-opts.md", "w");
		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Media Import");
		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
		PrintImportUsage();
		gf_fclose(helpout);

		helpout = gf_fopen("mp4box-dash-opts.md", "w");
		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Media DASH");
		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
		PrintDASHUsage();
		gf_fclose(helpout);

		helpout = gf_fopen("mp4box-dump-opts.md", "w");
		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Media Dump and Export");
		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
		PrintExtractUsage();
		PrintDumpUsage();
		gf_fclose(helpout);

		helpout = gf_fopen("mp4box-meta-opts.md", "w");
		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Meta and HEIF/IFF");
		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
		PrintMetaUsage();
		gf_fclose(helpout);


		helpout = gf_fopen("mp4box-scene-opts.md", "w");
		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Scene Description");
		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
		PrintEncodeUsage();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
		PrintLiveUsage();
#endif
		PrintSWFUsage();
		gf_fclose(helpout);

		helpout = gf_fopen("mp4box-other-opts.md", "w");
		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Other Features");
		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
		PrintHintUsage();
		PrintTags();
		gf_fclose(helpout);
	}
	//gen man
	else {
		help_flags = GF_PRINTARG_MAN;
		helpout = gf_fopen("mp4box.1", "w");


		fprintf(helpout, ".TH MP4Box 1 2019 MP4Box GPAC\n");
		fprintf(helpout, ".\n.SH NAME\n.LP\nMP4Box \\- GPAC command-line media packager\n.SH SYNOPSIS\n.LP\n.B MP4Box\n.RI [options] \\ [file] \\ [options]\n.br\n.\n");

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
		PrintTags();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
		PrintLiveUsage();
#endif

		fprintf(helpout, ".SH EXAMPLES\n.TP\nBasic and advanced examples are available at https://wiki.gpac.io/MP4Box\n");
		fprintf(helpout, ".SH MORE\n.LP\nAuthors: GPAC developers, see git repo history (-log)\n"
		".br\nFor bug reports, feature requests, more information and source code, visit https://github.com/gpac/gpac\n"
		".br\nbuild: %s\n"
		".br\nCopyright: %s\n.br\n"
		".SH SEE ALSO\n"
		".LP\ngpac(1), MP4Client(1)\n", gf_gpac_version(), gf_gpac_copyright());

		gf_fclose(helpout);
	}
	return 1;
}