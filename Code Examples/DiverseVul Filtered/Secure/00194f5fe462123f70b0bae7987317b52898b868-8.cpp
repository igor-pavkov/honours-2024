GF_Err gf_media_export_saf(GF_MediaExporter *dumper)
{
#ifndef GPAC_DISABLE_SAF
	u32 count, i, s_count, di, tot_samp, samp_done;
	char out_file[GF_MAX_PATH];
	GF_SAFMuxer *mux;
	u8 *data;
	u32 size;
	Bool is_stdout = 0;
	FILE *saf_f;
	SAFInfo safs[1024];

	if (dumper->flags & GF_EXPORT_PROBE_ONLY) return GF_OK;

	s_count = tot_samp = 0;

	mux = gf_saf_mux_new();
	count = gf_isom_get_track_count(dumper->file);
	for (i=0; i<count; i++) {
		u32 time_scale, mtype, stream_id;
		GF_ESD *esd;
		mtype = gf_isom_get_media_type(dumper->file, i+1);
		if (mtype==GF_ISOM_MEDIA_OD) continue;
		if (mtype==GF_ISOM_MEDIA_HINT) continue;

		time_scale = gf_isom_get_media_timescale(dumper->file, i+1);
		esd = gf_isom_get_esd(dumper->file, i+1, 1);
		if (esd) {
			stream_id = gf_isom_find_od_id_for_track(dumper->file, i+1);
			if (!stream_id) stream_id = esd->ESID;

			/*translate OD IDs to ESIDs !!*/
			if (esd->decoderConfig->decoderSpecificInfo) {
				gf_saf_mux_stream_add(mux, stream_id, time_scale, esd->decoderConfig->bufferSizeDB, esd->decoderConfig->streamType, esd->decoderConfig->objectTypeIndication, NULL, esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, esd->URLString);
			} else {
				gf_saf_mux_stream_add(mux, stream_id, time_scale, esd->decoderConfig->bufferSizeDB, esd->decoderConfig->streamType, esd->decoderConfig->objectTypeIndication, NULL, NULL, 0, esd->URLString);
			}
			gf_odf_desc_del((GF_Descriptor *)esd);
		} else {
			char *mime = NULL;
			switch (gf_isom_get_media_subtype(dumper->file, i+1, 1)) {
			case GF_ISOM_SUBTYPE_3GP_H263:
				mime = "video/h263";
				break;
			case GF_ISOM_SUBTYPE_3GP_AMR:
				mime = "audio/amr";
				break;
			case GF_ISOM_SUBTYPE_3GP_AMR_WB:
				mime = "audio/amr-wb";
				break;
			case GF_ISOM_SUBTYPE_3GP_EVRC:
				mime = "audio/evrc";
				break;
			case GF_ISOM_SUBTYPE_3GP_QCELP:
				mime = "audio/qcelp";
				break;
			case GF_ISOM_SUBTYPE_3GP_SMV:
				mime = "audio/smv";
				break;
			}
			if (!mime) continue;
			stream_id = gf_isom_get_track_id(dumper->file, i+1);
			gf_saf_mux_stream_add(mux, stream_id, time_scale, 0, 0xFF, 0xFF, mime, NULL, 0, NULL);
		}

		safs[s_count].track_num = i+1;
		safs[s_count].stream_id = stream_id;
		safs[s_count].nb_samp = gf_isom_get_sample_count(dumper->file, i+1);
		safs[s_count].last_sample = 0;

		tot_samp += safs[s_count].nb_samp;

		s_count++;
	}

	if (!s_count) {
		gf_export_message(dumper, GF_OK, "No tracks available for SAF muxing");
		gf_saf_mux_del(mux);
		return GF_OK;
	}
	gf_export_message(dumper, GF_OK, "SAF: Multiplexing %d tracks", s_count);

	if (dumper->out_name && !strcmp(dumper->out_name, "std"))
		is_stdout = 1;
	strcpy(out_file, dumper->out_name ? dumper->out_name : "");
	strcat(out_file, ".saf");
	saf_f = is_stdout ? stdout : gf_fopen(out_file, "wb");

	samp_done = 0;
	while (samp_done<tot_samp) {
		for (i=0; i<s_count; i++) {
			GF_ISOSample *samp;
			if (safs[i].last_sample==safs[i].nb_samp) continue;
			samp = gf_isom_get_sample(dumper->file, safs[i].track_num, safs[i].last_sample + 1, &di);
			gf_saf_mux_add_au(mux, safs[i].stream_id, (u32) (samp->DTS+samp->CTS_Offset), samp->data, samp->dataLength, (samp->IsRAP==RAP) ? 1 : 0);
			/*data is kept by muxer!!*/
			gf_free(samp);
			safs[i].last_sample++;
			samp_done ++;
		}
		while (1) {
			gf_saf_mux_for_time(mux, (u32) -1, 0, &data, &size);
			if (!data) break;
			gf_fwrite(data, size, saf_f);
			gf_free(data);
		}
		gf_set_progress("SAF Export", samp_done, tot_samp);
		if (dumper->flags & GF_EXPORT_DO_ABORT) break;
	}
	gf_saf_mux_for_time(mux, (u32) -1, 1, &data, &size);
	if (data) {
		gf_fwrite(data, size, saf_f);
		gf_free(data);
	}
	if (!is_stdout)
		gf_fclose(saf_f);

	gf_saf_mux_del(mux);
	return GF_OK;
#else
	return GF_NOT_SUPPORTED;
#endif
}