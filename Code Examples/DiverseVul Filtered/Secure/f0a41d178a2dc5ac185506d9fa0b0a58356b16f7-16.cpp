void gf_isom_reset_sample_count(GF_ISOFile *movie)
{
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	u32 i;
	if (!movie) return;
	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);
		trak->Media->information->sampleTable->SampleSize->sampleCount = 0;
		trak->sample_count_at_seg_start = 0;
	}
	movie->NextMoofNumber = 0;
#endif
}