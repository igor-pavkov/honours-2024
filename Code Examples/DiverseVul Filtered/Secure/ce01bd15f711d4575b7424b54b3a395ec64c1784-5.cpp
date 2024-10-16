void dump_isom_rtp(GF_ISOFile *file, char *inName, Bool is_final_name)
{
	u32 i, j, size;
	FILE *dump;
	const char *sdp;

	if (inName) {
		char szBuf[1024];
		strcpy(szBuf, inName);
		if (!is_final_name) strcat(szBuf, "_rtp.xml");
		dump = gf_fopen(szBuf, "wt");
		if (!dump) {
			fprintf(stderr, "Failed to open %s\n", szBuf);
			return;
		}
	} else {
		dump = stdout;
	}

	fprintf(dump, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fprintf(dump, "<!-- MP4Box RTP trace -->\n");
	fprintf(dump, "<RTPFile>\n");

	for (i=0; i<gf_isom_get_track_count(file); i++) {
		if (gf_isom_get_media_type(file, i+1) != GF_ISOM_MEDIA_HINT) continue;

		fprintf(dump, "<RTPHintTrack trackID=\"%d\">\n", gf_isom_get_track_id(file, i+1));
		gf_isom_sdp_track_get(file, i+1, &sdp, &size);
		fprintf(dump, "<SDPInfo>%s</SDPInfo>", sdp);

#ifndef GPAC_DISABLE_ISOM_HINTING
		for (j=0; j<gf_isom_get_sample_count(file, i+1); j++) {
			gf_isom_dump_hint_sample(file, i+1, j+1, dump);
		}
#endif
		fprintf(dump, "</RTPHintTrack>\n");
	}
	fprintf(dump, "</RTPFile>\n");
	if (inName) gf_fclose(dump);
}