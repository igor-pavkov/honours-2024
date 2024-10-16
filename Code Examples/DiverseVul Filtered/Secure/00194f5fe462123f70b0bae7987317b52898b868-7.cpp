static GF_Err gf_dump_to_vobsub(GF_MediaExporter *dumper, char *szName, u32 track, char *dsi, u32 dsiSize)
{
#ifndef GPAC_DISABLE_VOBSUB
	FILE *fidx, *fsub;
	u32 width, height, i, count, di;
	GF_ISOSample *samp;
	char *lang = NULL;

	if (!szName) {
		szName = gf_file_basename(gf_isom_get_filename(dumper->file));
		if (!szName) return GF_BAD_PARAM;
	}
	/* Check decoder specific information (palette) size - should be 64 */
	if (!dsi || (dsiSize != 64)) {
		return gf_export_message(dumper, GF_CORRUPTED_DATA, "Invalid decoder specific info size - must be 64 but is %d", dsiSize);
	}

	/* Create an idx file */
	if (!gf_file_ext_start(szName)) {
		char szPath[GF_MAX_PATH];
		strcpy(szPath, szName);
		strcat(szPath, ".idx");
		fidx = gf_fopen(szPath, "wb");
	 } else {
		fidx = gf_fopen(szName, "wb");
	}
	if (!fidx) {
		return gf_export_message(dumper, GF_IO_ERR, "Error opening %s for writing - check disk access & permissions", szName);
	}

	/* Create a sub file */
	char *ext = gf_file_ext_start(szName);
	if (ext && (!stricmp(ext, ".idx") || !stricmp(ext, ".sub")) ) {
		ext[0] = 0;
	}
	szName = strcat(szName, ".sub");
	fsub = gf_fopen(szName, "wb");
	if (!fsub) {
		gf_fclose(fidx);
		return gf_export_message(dumper, GF_IO_ERR, "Error opening %s for writing - check disk access & permissions", szName);
	}

	/* Retrieve original subpicture resolution */
	gf_isom_get_track_layout_info(dumper->file, track, &width, &height, NULL, NULL, NULL);

	/* Write header */
	gf_fputs("# VobSub index file, v7 (do not modify this line!)\n#\n", fidx);

	/* Write original subpicture resolution */
	gf_fprintf(fidx, "size: %ux%u\n", width, height);

	/* Write palette */
	gf_fputs("palette:", fidx);
	for (i = 0; i < 16; i++) {
		s32 y, u, v, r, g, b;

		y = (s32)(u8)dsi[(i<<2)+1] - 0x10;
		u = (s32)(u8)dsi[(i<<2)+3] - 0x80;
		v = (s32)(u8)dsi[(i<<2)+2] - 0x80;
		r = (298 * y           + 409 * v + 128) >> 8;
		g = (298 * y - 100 * u - 208 * v + 128) >> 8;
		b = (298 * y + 516 * u           + 128) >> 8;

		if (i) gf_fputc(',', fidx);

#define CLIP(x) (((x) >= 0) ? (((x) < 256) ? (x) : 255) : 0)
		gf_fprintf(fidx, " %02x%02x%02x", CLIP(r), CLIP(g), CLIP(b));
#undef CLIP
	}
	gf_fputc('\n', fidx);

	/* Write some other useful values */
	gf_fputs("# ON: displays only forced subtitles, OFF: shows everything\n", fidx);
	gf_fputs("forced subs: OFF\n\n", fidx);

	/* Write current language index */
	gf_fputs("# Language index in use\nlangidx: 0\n", fidx);

	/* Write language header */
	gf_isom_get_media_language(dumper->file, track, &lang);
	gf_fprintf(fidx, "id: %s, index: 0\n", vobsub_lang_id(lang));
	gf_free(lang);

	/* Retrieve sample count */
	count = gf_isom_get_sample_count(dumper->file, track);

	/* Process samples (skip first - because it is special) */
	for (i = 2; i <= count; i++)
	{
		u64 dts;
		u32 hh, mm, ss, ms;

		samp = gf_isom_get_sample(dumper->file, track, i, &di);
		if (!samp) {
			break;
		}

		dts = samp->DTS / 90;
		ms  = (u32)(dts % 1000);
		dts = dts / 1000;
		ss  = (u32)(dts % 60);
		dts = dts / 60;
		mm  = (u32)(dts % 60);
		hh  = (u32)(dts / 60);
		gf_fprintf(fidx, "timestamp: %02u:%02u:%02u:%03u, filepos: %09"LLX_SUF"\n", hh, mm, ss, ms, gf_ftell(fsub));
		if (vobsub_packetize_subpicture(fsub, samp->DTS, samp->data, samp->dataLength) != GF_OK) {
			gf_isom_sample_del(&samp);
			gf_fclose(fsub);
			gf_fclose(fidx);
			return gf_export_message(dumper, GF_IO_ERR, "Unable packetize subpicture into file %s\n", szName);
		}

		gf_isom_sample_del(&samp);
		gf_set_progress("VobSub Export", i + 1, count);

		if (dumper->flags & GF_EXPORT_DO_ABORT) {
			break;
		}
	}

	/* Delete sample if any */
	if (samp) {
		gf_isom_sample_del(&samp);
	}

	gf_fclose(fsub);
	gf_fclose(fidx);

	return GF_OK;
#else
	return GF_NOT_SUPPORTED;
#endif
}