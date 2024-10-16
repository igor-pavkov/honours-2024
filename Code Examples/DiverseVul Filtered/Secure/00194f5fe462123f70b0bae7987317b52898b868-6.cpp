GF_Err gf_media_export_isom(GF_MediaExporter *dumper)
{
	GF_ISOFile *outfile;
	GF_Err e;
	Bool add_to_iod, is_stdout;
	char szName[1000];
	u32 track;
	GF_ISOOpenMode mode;

	if (!(track = gf_isom_get_track_by_id(dumper->file, dumper->trackID))) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("Wrong track ID %d for file %s \n", dumper->trackID, gf_isom_get_filename(dumper->file)));
		return GF_BAD_PARAM;
	}
	if (gf_isom_get_media_type(dumper->file, dumper->trackID)==GF_ISOM_MEDIA_OD) {
		return gf_export_message(dumper, GF_BAD_PARAM, "Cannot extract OD track, result is  meaningless");
	}

	if (dumper->flags & GF_EXPORT_PROBE_ONLY) {
		dumper->flags |= GF_EXPORT_MERGE;
		return GF_OK;
	}
	if (dumper->out_name && gf_file_ext_start(dumper->out_name)) {
		strcpy(szName, dumper->out_name);
	} else {
		char *ext = (char *) gf_isom_get_filename(dumper->file);
		if (ext) ext = gf_file_ext_start(ext);
		sprintf(szName, "%s%s", dumper->out_name, ext ? ext : ".mp4");
	}
	is_stdout = (dumper->out_name && !strcmp(dumper->out_name, "std")) ? 1 : 0;
	add_to_iod = 1;
	mode = GF_ISOM_WRITE_EDIT;
	if (!is_stdout && (dumper->flags & GF_EXPORT_MERGE)) {
		FILE *t = gf_fopen(szName, "rb");
		if (t) {
			add_to_iod = 0;
			mode = GF_ISOM_OPEN_EDIT;
			gf_fclose(t);
		}
	}
	outfile = gf_isom_open(is_stdout ? "std" : szName, mode, NULL);

	if (mode == GF_ISOM_WRITE_EDIT) {
		gf_isom_set_pl_indication(outfile, GF_ISOM_PL_AUDIO, 0xFF);
		gf_isom_set_pl_indication(outfile, GF_ISOM_PL_VISUAL, 0xFF);
		gf_isom_set_pl_indication(outfile, GF_ISOM_PL_GRAPHICS, 0xFF);
		gf_isom_set_pl_indication(outfile, GF_ISOM_PL_SCENE, 0xFF);
		gf_isom_set_pl_indication(outfile, GF_ISOM_PL_OD, 0xFF);
		gf_isom_set_pl_indication(outfile, GF_ISOM_PL_MPEGJ, 0xFF);
	}
	if (gf_isom_has_keep_utc_times(dumper->file)) {
		u64 cdate, mdate;
		gf_isom_get_creation_time(dumper->file, &cdate, &mdate);
		gf_isom_set_creation_time(outfile, cdate, mdate);
	}

	e = gf_export_isom_copy_track(dumper, dumper->file, track, outfile, 1, add_to_iod);
	if (!add_to_iod) {
		u32 i;
		for (i=0; i<gf_isom_get_track_count(outfile); i++) {
			gf_isom_remove_track_from_root_od(outfile, i+1);
		}
	}

	if (gf_isom_has_keep_utc_times(dumper->file))
		gf_isom_keep_utc_times(outfile, GF_TRUE);

	if (e) gf_isom_delete(outfile);
	else gf_isom_close(outfile);

	return e;
}