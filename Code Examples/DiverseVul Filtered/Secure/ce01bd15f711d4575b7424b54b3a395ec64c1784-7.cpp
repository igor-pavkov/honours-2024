void dump_isom_timestamps(GF_ISOFile *file, char *inName, Bool is_final_name, u32 dump_mode)
{
	u32 i, j, k, count;
	Bool has_ctts_error, is_fragmented=GF_FALSE;
	FILE *dump;
	Bool skip_offset = ((dump_mode==2) || (dump_mode==4)) ? GF_TRUE : GF_FALSE;
	Bool check_ts = ((dump_mode==3) || (dump_mode==4)) ? GF_TRUE : GF_FALSE;
	struct _ts_info {
		u64 dts;
		s64 cts;
	};
	struct _ts_info *timings = NULL;
	u32 nb_timings=0, nb_timings_alloc = 0;


	if (inName) {
		char szBuf[1024];
		strcpy(szBuf, inName);
		if (!is_final_name) strcat(szBuf, "_ts.txt");
		dump = gf_fopen(szBuf, "wt");
		if (!dump) {
			fprintf(stderr, "Failed to open %s\n", szBuf);
			return;
		}
	} else {
		dump = stdout;
	}
	if (gf_isom_is_fragmented(file))
		is_fragmented = GF_TRUE;

	has_ctts_error = GF_FALSE;
	for (i=0; i<gf_isom_get_track_count(file); i++) {
		s64 cts_dts_shift = gf_isom_get_cts_to_dts_shift(file, i+1);
		u32 has_cts_offset = gf_isom_has_time_offset(file, i+1);

		fprintf(dump, "#dumping track ID %d timing:\n", gf_isom_get_track_id(file, i + 1));
		fprintf(dump, "Num\tDTS\tCTS\tSize\tRAP%s\tisLeading\tDependsOn\tDependedOn\tRedundant\tRAP-SampleGroup\tRoll-SampleGroup\tRoll-Distance", skip_offset ? "" : "\tOffset");
		if (is_fragmented) {
			fprintf(dump, "\tfrag_start");
		}
		fprintf(dump, "\n");


		count = gf_isom_get_sample_count(file, i+1);
		if (has_cts_offset && check_ts) {
			if (nb_timings_alloc<count) {
				nb_timings_alloc = count;
				timings = gf_realloc(timings, sizeof (struct _ts_info) * count);
			}
			nb_timings = 0;
		}

		for (j=0; j<count; j++) {
			s64 cts;
			u64 dts, offset;
			u32 isLeading, dependsOn, dependedOn, redundant;
			Bool is_rap;
			GF_ISOSampleRollType roll_type;
			s32 roll_distance;
			u32 index;
			GF_ISOSample *samp = gf_isom_get_sample_info(file, i+1, j+1, &index, &offset);
			if (!samp) {
				fprintf(dump, " SAMPLE #%d IN TRACK #%d NOT THERE !!!\n", j+1, i+1);
				continue;
			}
			gf_isom_get_sample_flags(file, i+1, j+1, &isLeading, &dependsOn, &dependedOn, &redundant);
			gf_isom_get_sample_rap_roll_info(file, i+1, j+1, &is_rap, &roll_type, &roll_distance);
			dts = samp->DTS;
			cts = dts + (s32) samp->CTS_Offset;
			fprintf(dump, "Sample %d\tDTS "LLU"\tCTS "LLD"\t%d\t%d", j+1, dts, cts, samp->dataLength, samp->IsRAP);

			if (!skip_offset)
				fprintf(dump, "\t"LLU, offset);

			fprintf(dump, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d", isLeading, dependsOn, dependedOn, redundant, is_rap, roll_type, roll_distance);

			if (cts< (s64) dts) {
				if (has_cts_offset==2) {
					if (cts_dts_shift && (cts+cts_dts_shift < (s64) dts)) {
						fprintf(dump, " #NEGATIVE CTS OFFSET!!!");
						has_ctts_error = 1;
					} else if (!cts_dts_shift) {
						fprintf(dump, " #possible negative CTS offset (no cslg in file)");
					}
				} else {
					fprintf(dump, " #NEGATIVE CTS OFFSET!!!");
					has_ctts_error = 1;
				}
			}
			if (has_cts_offset && check_ts) {
				for (k=0; k<nb_timings; k++) {

					if (timings[k].dts==dts) {
						fprintf(dump, " #SAME DTS USED!!!");
						has_ctts_error = 1;
					}
					if (timings[k].cts==cts) {
						fprintf(dump, " #SAME CTS USED!!! ");
						has_ctts_error = 1;
					}
				}
				timings[nb_timings].dts = dts;
				timings[nb_timings].cts = cts;
				nb_timings++;
			}

			gf_isom_sample_del(&samp);

			if (is_fragmented) {
				fprintf(dump, "\t%d", gf_isom_sample_is_fragment_start(file, i+1, j+1, NULL) );
			}
			fprintf(dump, "\n");
			gf_set_progress("Dumping track timing", j+1, count);
		}
		fprintf(dump, "\n\n");
		gf_set_progress("Dumping track timing", count, count);
	}
	if (timings) gf_free(timings);

	if (inName) gf_fclose(dump);
	if (has_ctts_error) fprintf(stderr, "\tFile has CTTS table errors\n");
}