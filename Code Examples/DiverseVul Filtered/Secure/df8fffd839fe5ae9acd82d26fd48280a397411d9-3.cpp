u32 gf_isom_get_tracknum_from_id(GF_MovieBox *moov, GF_ISOTrackID trackID)
{
	u32 i;
	GF_TrackBox *trak;
	i=0;
	while ((trak = (GF_TrackBox *)gf_list_enum(moov->trackList, &i))) {
		if (trak->Header->trackID == trackID) return i;
	}
	return 0;
}