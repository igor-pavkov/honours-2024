#endif

static void dump_isom_obu(GF_ISOFile *file, GF_ISOTrackID trackID, FILE *dump, Bool dump_crc)
{
#ifndef GPAC_DISABLE_AV_PARSERS
	u32 i, count, track, timescale;
	AV1State av1;
	ObuType obu_type;
	u64 obu_size;
	u32 hdr_size;
	GF_BitStream *bs;
	u32 idx;

	track = gf_isom_get_track_by_id(file, trackID);

	gf_av1_init_state(&av1);
	av1.config = gf_isom_av1_config_get(file, track, 1);
	if (!av1.config) {
		fprintf(stderr, "Error: Track #%d is not AV1!\n", trackID);
		return;
	}

	count = gf_isom_get_sample_count(file, track);
	timescale = gf_isom_get_media_timescale(file, track);

	fprintf(dump, "<OBUTrack trackID=\"%d\" SampleCount=\"%d\" TimeScale=\"%d\">\n", trackID, count, timescale);

	fprintf(dump, " <OBUConfig>\n");

	for (i=0; i<gf_list_count(av1.config->obu_array); i++) {
		GF_AV1_OBUArrayEntry *obu = gf_list_get(av1.config->obu_array, i);
		bs = gf_bs_new(obu->obu, (u32) obu->obu_length, GF_BITSTREAM_READ);
		gf_media_aom_av1_parse_obu(bs, &obu_type, &obu_size, &hdr_size, &av1);
		gf_inspect_dump_obu(dump, &av1, obu->obu, obu->obu_length, obu_type, obu_size, hdr_size, dump_crc);
		gf_bs_del(bs);
	}
	fprintf(dump, " </OBUConfig>\n");

	fprintf(dump, " <OBUSamples>\n");

	for (i=0; i<count; i++) {
		u64 dts, cts;
		u32 size;
		u8 *ptr;
		GF_ISOSample *samp = gf_isom_get_sample(file, track, i+1, NULL);
		if (!samp) {
			fprintf(dump, "<!-- Unable to fetch sample %d -->\n", i+1);
			continue;
		}
		dts = samp->DTS;
		cts = dts + (s32) samp->CTS_Offset;

		fprintf(dump, "  <Sample number=\"%d\" DTS=\""LLD"\" CTS=\""LLD"\" size=\"%d\" RAP=\"%d\" >\n", i+1, dts, cts, samp->dataLength, samp->IsRAP);
		if (cts<dts) fprintf(dump, "<!-- NEGATIVE CTS OFFSET! -->\n");

		idx = 1;
		ptr = samp->data;
		size = samp->dataLength;

		bs = gf_bs_new(ptr, size, GF_BITSTREAM_READ);
		while (size) {
			gf_media_aom_av1_parse_obu(bs, &obu_type, &obu_size, &hdr_size, &av1);
			if (obu_size > size) {
				fprintf(dump, "   <!-- OBU number %d is corrupted: size is %d but only %d remains -->\n", idx, (u32) obu_size, size);
				break;
			}
			gf_inspect_dump_obu(dump, &av1, ptr, obu_size, obu_type, obu_size, hdr_size, dump_crc);
			ptr += obu_size;
			size -= (u32)obu_size;
			idx++;
		}
		gf_bs_del(bs);
		fprintf(dump, "  </Sample>\n");
		gf_isom_sample_del(&samp);

		fprintf(dump, "\n");
		gf_set_progress("Analysing Track OBUs", i+1, count);
	}
	fprintf(dump, " </OBUSamples>\n");
	fprintf(dump, "</OBUTrack>\n");

	if (av1.config) gf_odf_av1_cfg_del(av1.config);
	gf_av1_reset_state(&av1, GF_TRUE);