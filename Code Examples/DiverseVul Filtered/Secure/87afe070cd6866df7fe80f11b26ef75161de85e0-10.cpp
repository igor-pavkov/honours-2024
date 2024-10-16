static GF_Err do_export_tracks()
{
	GF_Err e;
	u32 i;
	char szFile[GF_MAX_PATH+24];
	GF_MediaExporter mdump;
	for (i=0; i<nb_track_act; i++) {
		u32 j;
		TrackAction *tka = &tracks[i];
		if (tka->act_type != TRAC_ACTION_RAW_EXTRACT) continue;
		memset(&mdump, 0, sizeof(mdump));
		mdump.file = file;
		mdump.flags = tka->dump_type;
		mdump.trackID = tka->trackID;
		mdump.sample_num = tka->sample_num;
		if (tka->out_name) {
			mdump.out_name = tka->out_name;
		} else if (outName) {
			mdump.out_name = outName;
			mdump.flags |= GF_EXPORT_MERGE;
			/*don't infer extension on user-given filename*/
			mdump.flags |= GF_EXPORT_NO_FILE_EXT;
		} else if (mdump.trackID) {
			sprintf(szFile, "%s_track%d", outfile, mdump.trackID);
			mdump.out_name = szFile;
		} else {
			sprintf(szFile, "%s_export", outfile);
			mdump.out_name = szFile;
		}
		if (tka->trackID==(u32) -1) {
			for (j=0; j<gf_isom_get_track_count(file); j++) {
				mdump.trackID = gf_isom_get_track_id(file, j+1);
				sprintf(szFile, "%s_track%d", outfile, mdump.trackID);
				mdump.out_name = szFile;
				mdump.print_stats_graph = fs_dump_flags;
				e = gf_media_export(&mdump);
				if (e) return e;
			}
		} else {
			mdump.print_stats_graph = fs_dump_flags;
			e = gf_media_export(&mdump);
			if (e) return e;
		}
	}
	return GF_OK;
}