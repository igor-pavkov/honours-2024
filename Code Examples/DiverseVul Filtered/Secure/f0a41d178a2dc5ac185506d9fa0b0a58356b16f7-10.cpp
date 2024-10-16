GF_Err gf_isom_open_segment(GF_ISOFile *movie, const char *fileName, u64 start_range, u64 end_range, GF_ISOSegOpenMode flags)
{
#ifdef	GPAC_DISABLE_ISOM_FRAGMENTS
	return GF_NOT_SUPPORTED;
#else
	u64 MissingBytes;
	GF_Err e;
	u32 i;
	Bool segment_map_assigned = GF_FALSE;
	Bool is_scalable_segment = (flags & GF_ISOM_SEGMENT_SCALABLE_FLAG) ? GF_TRUE : GF_FALSE;
	Bool no_order_check = (flags & GF_ISOM_SEGMENT_NO_ORDER_FLAG) ? GF_TRUE: GF_FALSE;
	GF_DataMap *tmp = NULL;
	GF_DataMap *orig_file_map = NULL;
	if (!movie || !movie->moov || !movie->moov->mvex) return GF_BAD_PARAM;
	if (movie->openMode != GF_ISOM_OPEN_READ) return GF_BAD_PARAM;

	/*this is a scalable segment - use a temp data map for the associated track(s) but do NOT touch the movie file map*/
	if (is_scalable_segment) {
		tmp = NULL;
		e = gf_isom_datamap_new(fileName, NULL, GF_ISOM_DATA_MAP_READ_ONLY, &tmp);
		if (e) return e;

		orig_file_map = movie->movieFileMap;
		movie->movieFileMap = tmp;
	} else {
		if (movie->movieFileMap)
			gf_isom_release_segment(movie, GF_FALSE);

		e = gf_isom_datamap_new(fileName, NULL, GF_ISOM_DATA_MAP_READ_ONLY, &movie->movieFileMap);
		if (e) return e;
	}
	movie->moov->compressed_diff = 0;
	movie->current_top_box_start = 0;

	if (start_range || end_range) {
		if (end_range > start_range) {
			gf_bs_seek(movie->movieFileMap->bs, end_range+1);
			gf_bs_truncate(movie->movieFileMap->bs);
		}
		gf_bs_seek(movie->movieFileMap->bs, start_range);
		movie->current_top_box_start = start_range;
	}

	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);

		if (!is_scalable_segment) {
			/*reset data handler to new segment*/
			if (trak->Media->information->dataHandler == NULL) {
				trak->Media->information->dataHandler = movie->movieFileMap;
			}
		} else {
			trak->present_in_scalable_segment = GF_FALSE;
		}
	}
	if (no_order_check) movie->NextMoofNumber = 0;

	//ok parse root boxes
	e = gf_isom_parse_movie_boxes(movie, NULL, &MissingBytes, GF_TRUE);

	if (!is_scalable_segment)
		return e;

	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);
		if (trak->present_in_scalable_segment) {
			/*store the temp dataHandler into scalableDataHandler so that it will not be destroyed
			if we append another representation - destruction of this data handler is done in release_segment*/
			trak->Media->information->scalableDataHandler = tmp;
			if (!segment_map_assigned) {
				trak->Media->information->scalableDataHandler = tmp;
				segment_map_assigned = GF_TRUE;
			}
			//and update the regular dataHandler for the Media_GetSample function
			trak->Media->information->dataHandler = tmp;
		}
	}
	movie->movieFileMap = 	orig_file_map;
	return e;
#endif
}