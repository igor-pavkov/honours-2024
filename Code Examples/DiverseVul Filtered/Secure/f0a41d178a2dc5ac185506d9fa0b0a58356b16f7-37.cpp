GF_Err gf_isom_release_segment(GF_ISOFile *movie, Bool reset_tables)
{
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	u32 i, j, base_track_sample_count;
	Bool has_scalable;
	GF_Box *a;
	if (!movie || !movie->moov || !movie->moov->mvex) return GF_BAD_PARAM;
	has_scalable = gf_isom_needs_layer_reconstruction(movie);
	base_track_sample_count = 0;
	movie->moov->compressed_diff = 0;
	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);
		trak->first_traf_merged = GF_FALSE;
		if (trak->Media->information->dataHandler == movie->movieFileMap) {
			trak->Media->information->dataHandler = NULL;
		}
		if (trak->Media->information->scalableDataHandler == movie->movieFileMap) {
			trak->Media->information->scalableDataHandler = NULL;
		} else {
			if (trak->Media->information->scalableDataHandler==trak->Media->information->dataHandler)
				trak->Media->information->dataHandler = NULL;

			gf_isom_datamap_del(trak->Media->information->scalableDataHandler);
			trak->Media->information->scalableDataHandler = NULL;
		}


		if (reset_tables) {
			u32 dur;
			u64 dts;
			GF_SampleTableBox *stbl = trak->Media->information->sampleTable;

			if (has_scalable) {
				//check if the base reference is in the file - if not, do not consider the track is scalable.
				if (trak->nb_base_refs) {
					u32 on_track=0;
					GF_TrackBox *base;
					gf_isom_get_reference(movie, i+1, GF_ISOM_REF_BASE, 1, &on_track);

					base = gf_isom_get_track_from_file(movie, on_track);
					if (!base) {
						base_track_sample_count=0;
					} else {
						base_track_sample_count = base->Media->information->sampleTable->SampleSize->sampleCount;
					}
				}
			}

			trak->sample_count_at_seg_start += base_track_sample_count ? base_track_sample_count : stbl->SampleSize->sampleCount;

			if (trak->sample_count_at_seg_start) {
				GF_Err e;
				e = stbl_GetSampleDTS_and_Duration(stbl->TimeToSample, stbl->SampleSize->sampleCount, &dts, &dur);
				if (e == GF_OK) {
					trak->dts_at_seg_start += dts + dur;
				}
			}

			gf_isom_recreate_tables(trak);


#if 0 // TO CHECK
			j = ptr->nb_stbl_boxes;
			while ((a = (GF_Box *)gf_list_enum(stbl->child_boxes, &j))) {
				gf_isom_box_del_parent(&stbl->child_boxes, a);
				j--;
			}
#endif
		}


		if (movie->has_pssh_moof) {
			j = 0;
			while ((a = (GF_Box *)gf_list_enum(movie->moov->child_boxes, &j))) {
				if (a->type == GF_ISOM_BOX_TYPE_PSSH) {
					GF_ProtectionSystemHeaderBox *pssh = (GF_ProtectionSystemHeaderBox *)a;
					if (pssh->moof_defined) {
						gf_isom_box_del_parent(&movie->moov->child_boxes, a);
						j--;
					}
				}
			}
			movie->has_pssh_moof = GF_FALSE;
		}
	}

	gf_isom_datamap_del(movie->movieFileMap);
	movie->movieFileMap = NULL;
#endif
	return GF_OK;
}