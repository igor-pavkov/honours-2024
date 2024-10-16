GF_TrackBox *gf_isom_get_track(GF_MovieBox *moov, u32 trackNumber)
{
	GF_TrackBox *trak;
	if (!moov || !trackNumber || (trackNumber > gf_list_count(moov->trackList))) return NULL;
	trak = (GF_TrackBox*)gf_list_get(moov->trackList, trackNumber - 1);
	return trak;

}