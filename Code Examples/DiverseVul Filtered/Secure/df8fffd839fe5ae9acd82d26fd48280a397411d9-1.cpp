GF_TrackBox *GetTrackbyID(GF_MovieBox *moov, GF_ISOTrackID TrackID)
{
	GF_TrackBox *trak;
	u32 i;
	if (!moov) return NULL;
	i=0;
	while ((trak = (GF_TrackBox *)gf_list_enum(moov->trackList, &i))) {
		if (trak->Header->trackID == TrackID) return trak;
	}
	return NULL;
}