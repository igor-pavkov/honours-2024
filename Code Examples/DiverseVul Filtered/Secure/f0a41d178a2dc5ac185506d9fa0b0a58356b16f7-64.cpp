GF_Err gf_isom_reset_tables(GF_ISOFile *movie, Bool reset_sample_count)
{
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	u32 i;

	if (!movie || !movie->moov || !movie->moov->mvex) return GF_BAD_PARAM;
	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		GF_TrackBox *trak = (GF_TrackBox *)gf_list_get(movie->moov->trackList, i);

		u32 dur;
		u64 dts;
		GF_SampleTableBox *stbl = trak->Media->information->sampleTable;

		trak->sample_count_at_seg_start += stbl->SampleSize->sampleCount;
		if (trak->sample_count_at_seg_start) {
			GF_Err e;
			e = stbl_GetSampleDTS_and_Duration(stbl->TimeToSample, stbl->SampleSize->sampleCount, &dts, &dur);
			if (e == GF_OK) {
				trak->dts_at_seg_start += dts + dur;
			}
		}

		//recreate all boxes
		gf_isom_recreate_tables(trak);

#if 0
		j = stbl->nb_stbl_boxes;
		while ((a = (GF_Box *)gf_list_enum(stbl->child_boxes, &j))) {
			gf_isom_box_del_parent(&stbl->child_boxes, a);
			j--;
		}
#endif

		if (reset_sample_count) {
			trak->Media->information->sampleTable->SampleSize->sampleCount = 0;
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
			trak->sample_count_at_seg_start = 0;
			trak->dts_at_seg_start = 0;
			trak->first_traf_merged = GF_FALSE;
#endif
		}

	}
	if (reset_sample_count) {
		movie->NextMoofNumber = 0;
	}
#endif
	return GF_OK;

}