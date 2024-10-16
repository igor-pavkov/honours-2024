static u32 do_import_sub()
{
	/* We import the subtitle file,
	   i.e. we parse it and store the content as samples of a 3GPP Timed Text track in an ISO file,
	   possibly for later export (e.g. when converting SRT to TTXT, ...) */
#ifndef GPAC_DISABLE_MEDIA_IMPORT
	GF_Err e;
	GF_MediaImporter import;
	/* Prepare the importer */
	file = gf_isom_open("ttxt_convert", GF_ISOM_OPEN_WRITE, NULL);
	if (timescale && file) gf_isom_set_timescale(file, timescale);

	memset(&import, 0, sizeof(GF_MediaImporter));
	import.dest = file;
	import.in_name = inName;
	/* Start the import */
	e = gf_media_import(&import);
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Error importing %s: %s\n", inName, gf_error_to_string(e)));
		gf_isom_delete(file);
		gf_file_delete("ttxt_convert");
		return mp4box_cleanup(1);
	}
	/* Prepare the export */
	strcpy(outfile, inName);
	if (strchr(outfile, '.')) {
		while (outfile[strlen(outfile)-1] != '.') outfile[strlen(outfile)-1] = 0;
		outfile[strlen(outfile)-1] = 0;
	}
#ifndef GPAC_DISABLE_ISOM_DUMP
	/* Start the export of the track #1, in the appropriate dump type, indicating it's a conversion */
	dump_isom_timed_text(file, gf_isom_get_track_id(file, 1),
						  dump_std ? NULL : (outName ? outName : outfile), outName ? GF_TRUE : GF_FALSE,
						  GF_TRUE,
						  (import_subtitle==2) ? GF_TEXTDUMPTYPE_SVG : (dump_srt ? GF_TEXTDUMPTYPE_SRT : GF_TEXTDUMPTYPE_TTXT));
#endif
	/* Clean the importer */
	gf_isom_delete(file);
	gf_file_delete("ttxt_convert");
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Error converting %s: %s\n", inName, gf_error_to_string(e)));
		return mp4box_cleanup(1);
	}
	return mp4box_cleanup(0);
#else
	M4_LOG(GF_LOG_ERROR, ("Feature not supported\n"));
	return mp4box_cleanup(1);
#endif
}