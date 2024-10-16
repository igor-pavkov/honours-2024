static u32 convert_mpd()
{
	GF_Err e;
	Bool remote = GF_FALSE;
	GF_MPD *mpd;
	char *mpd_base_url = NULL;
	if (!strnicmp(inName, "http://", 7) || !strnicmp(inName, "https://", 8)) {
#if !defined(GPAC_DISABLE_CORE_TOOLS)
		e = gf_dm_wget(inName, "tmp_main.m3u8", 0, 0, &mpd_base_url);
		if (e != GF_OK) {
			M4_LOG(GF_LOG_ERROR, ("Cannot retrieve M3U8 (%s): %s\n", inName, gf_error_to_string(e)));
			if (mpd_base_url) gf_free(mpd_base_url);
			return mp4box_cleanup(1);
		}
		remote = GF_TRUE;
#else
		gf_free(mpd_base_url);
		M4_LOG(GF_LOG_ERROR, ("HTTP Downloader disabled in this build\n"));
		return mp4box_cleanup(1);
#endif

		if (outName)
			strcpy(outfile, outName);
		else {
			const char *sep = gf_file_basename(inName);
			char *ext = gf_file_ext_start(sep);
			if (ext) ext[0] = 0;
			sprintf(outfile, "%s.mpd", sep);
			if (ext) ext[0] = '.';
		}
	} else {
		if (outName)
			strcpy(outfile, outName);
		else {
			char *dst = strdup(inName);
			char *ext = strstr(dst, ".m3u8");
			if (ext) ext[0] = 0;
			sprintf(outfile, "%s.mpd", dst);
			gf_free(dst);
		}
	}

	mpd = gf_mpd_new();
	if (!mpd) {
		e = GF_OUT_OF_MEM;
		M4_LOG(GF_LOG_ERROR, ("[DASH] Error: MPD creation problem %s\n", gf_error_to_string(e)));
		mp4box_cleanup(1);
	}
	FILE *f = gf_fopen(remote ? "tmp_main.m3u8" : inName, "r");
	u32 manif_type = 0;
	if (f) {
		char szDATA[1000];
		s32 read;
		szDATA[999]=0;
		read = (s32) gf_fread(szDATA, 999, f);
		if (read<0) read = 0;
		szDATA[read]=0;
		gf_fclose(f);
		if (strstr(szDATA, "SmoothStreamingMedia"))
			manif_type = 2;
		else if (strstr(szDATA, "#EXTM3U"))
			manif_type = 1;
	}

	if (manif_type==1) {
		e = gf_m3u8_to_mpd(remote ? "tmp_main.m3u8" : inName, mpd_base_url ? mpd_base_url : inName, outfile, 0, "video/mp2t", GF_TRUE, use_url_template, segment_timeline, NULL, mpd, GF_TRUE, GF_TRUE);
	} else if (manif_type==2) {
		e = gf_mpd_smooth_to_mpd(remote ? "tmp_main.m3u8" : inName, mpd, mpd_base_url ? mpd_base_url : inName);
	} else {
		e = GF_NOT_SUPPORTED;
	}
	if (!e)
		gf_mpd_write_file(mpd, outfile);

	if (mpd)
		gf_mpd_del(mpd);
	if (mpd_base_url)
		gf_free(mpd_base_url);

	if (remote) {
		gf_file_delete("tmp_main.m3u8");
	}
	if (e != GF_OK) {
		M4_LOG(GF_LOG_ERROR, ("Error converting %s (%s) to MPD (%s): %s\n", (manif_type==1) ? "HLS" : "Smooth",  inName, outfile, gf_error_to_string(e)));
		return mp4box_cleanup(1);
	} else {
		M4_LOG(GF_LOG_INFO, ("Done converting %s (%s) to MPD (%s)\n", (manif_type==1) ? "HLS" : "Smooth",  inName, outfile));
		return mp4box_cleanup(0);
	}
}