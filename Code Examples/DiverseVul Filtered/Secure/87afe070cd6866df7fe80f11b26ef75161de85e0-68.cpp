static GF_Err do_export_tracks_non_isobmf()
{
	u32 i;

	GF_MediaExporter mdump;
	char szFile[GF_MAX_PATH+24];
	for (i=0; i<nb_track_act; i++) {
		GF_Err e;
		TrackAction *tka = &tracks[i];
		if (tka->act_type != TRAC_ACTION_RAW_EXTRACT) continue;
		memset(&mdump, 0, sizeof(mdump));
		mdump.in_name = inName;
		mdump.flags = tka->dump_type;
		mdump.trackID = tka->trackID;
		mdump.track_type = tka->dump_track_type;
		mdump.sample_num = tka->sample_num;

		if (dump_std) {
			mdump.out_name = "std";
		}
		else if (outName) {
			mdump.out_name = outName;
			mdump.flags |= GF_EXPORT_MERGE;
		} else if (nb_track_act>1) {
			sprintf(szFile, "%s_track%d", outfile, mdump.trackID);
			mdump.out_name = szFile;
		} else {
			mdump.out_name = outfile;
		}
		mdump.print_stats_graph = fs_dump_flags;
		e = gf_media_export(&mdump);
		if (e) return e;
	}
	return GF_OK;
}