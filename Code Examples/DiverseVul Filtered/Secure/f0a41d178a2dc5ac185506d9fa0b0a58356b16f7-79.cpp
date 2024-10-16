GF_Err gf_isom_get_file_offset_for_time(GF_ISOFile *movie, Double start_time, u64 *max_offset)
{
	u32 i;
	u64 start_ts, cur_start_time;
	u64 offset=0;
	if (!movie || !movie->moov)
		return GF_BAD_PARAM;

	if (!movie->main_sidx) return GF_NOT_SUPPORTED;
	start_ts = (u64) (start_time * movie->main_sidx->timescale);
	cur_start_time = 0;
	offset = movie->main_sidx->first_offset + movie->main_sidx_end_pos;

	for (i=0; i<movie->main_sidx->nb_refs; i++) {
		if (cur_start_time >= start_ts) {
			*max_offset = offset;
			return GF_OK;
		}
		cur_start_time += movie->main_sidx->refs[i].subsegment_duration;
		offset += movie->main_sidx->refs[i].reference_size;
	}

	return GF_EOS;
}