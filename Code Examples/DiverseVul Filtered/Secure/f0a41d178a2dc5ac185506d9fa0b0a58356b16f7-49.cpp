GF_Err gf_isom_get_sidx_duration(GF_ISOFile *movie, u64 *sidx_dur, u32 *sidx_timescale)
{
	u64 dur=0;
	u32 i;
	if (!movie || !movie->moov || !sidx_timescale || !sidx_dur)
		return GF_BAD_PARAM;

	if (!movie->main_sidx) return GF_NOT_SUPPORTED;
	*sidx_timescale = movie->main_sidx->timescale;

	for (i=0; i<movie->main_sidx->nb_refs; i++) {
		dur += movie->main_sidx->refs[i].subsegment_duration;
	}
	*sidx_dur = dur;
	return GF_OK;
}