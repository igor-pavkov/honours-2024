static GF_Err do_scene_encode()
{
	GF_Err e;
	FILE *logs = NULL;
	if (do_scene_log) {
		char alogfile[GF_MAX_PATH];
		strcpy(alogfile, inName);
		if (strchr(alogfile, '.')) {
			while (alogfile[strlen(alogfile)-1] != '.') alogfile[strlen(alogfile)-1] = 0;
			alogfile[strlen(alogfile)-1] = 0;
		}
		strcat(alogfile, "_enc.logs");
		logs = gf_fopen(alogfile, "wt");
	}
	strcpy(outfile, outName ? outName : inName);
	if (strchr(outfile, '.')) {
		while (outfile[strlen(outfile)-1] != '.') outfile[strlen(outfile)-1] = 0;
		outfile[strlen(outfile)-1] = 0;
	}
	strcat(outfile, ".mp4");
	file = gf_isom_open(outfile, GF_ISOM_WRITE_EDIT, NULL);
	smenc_opts.mediaSource = mediaSource ? mediaSource : outfile;
	e = EncodeFile(inName, file, &smenc_opts, logs);
	if (logs) gf_fclose(logs);
	if (e) return e;
	do_save = GF_TRUE;
	if (do_saf) {
		do_save = GF_FALSE;
		open_edit = GF_FALSE;
	}
	return GF_OK;
}