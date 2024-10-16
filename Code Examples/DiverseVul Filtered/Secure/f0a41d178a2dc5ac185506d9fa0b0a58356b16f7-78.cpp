Bool gf_isom_sample_is_fragment_start(GF_ISOFile *movie, u32 trackNumber, u32 sampleNum, GF_ISOFragmentBoundaryInfo *frag_info)
{
	u32 i;
	GF_TrackBox *trak;
	GF_TrafToSampleMap *tmap;

	if (frag_info) memset(frag_info, 0, sizeof(GF_ISOFragmentBoundaryInfo));

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media) return GF_FALSE;
	if (!trak->Media->information->sampleTable->traf_map) return GF_FALSE;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sampleNum<=trak->sample_count_at_seg_start)
		return GF_FALSE;
	sampleNum -= trak->sample_count_at_seg_start;
#endif

	tmap = trak->Media->information->sampleTable->traf_map;
	if (!tmap) return GF_FALSE;
	for (i=0; i<tmap->nb_entries; i++) {
		GF_TrafMapEntry *finfo = &tmap->frag_starts[i];
		if (finfo->sample_num == sampleNum) {
			if (frag_info) {
				frag_info->frag_start = finfo->moof_start;
				frag_info->mdat_end = finfo->mdat_end;
				frag_info->moof_template = finfo->moof_template;
				frag_info->moof_template_size = finfo->moof_template_size;
				frag_info->seg_start_plus_one = finfo->seg_start_plus_one;
				frag_info->sidx_start = finfo->sidx_start;
				frag_info->sidx_end = finfo->sidx_end;
			}
			return GF_TRUE;
		}

		if (tmap->frag_starts[i].sample_num > sampleNum) return GF_FALSE;
	}
	return GF_FALSE;
}