u8 RequestTrack(GF_MovieBox *moov, GF_ISOTrackID TrackID)
{
	u32 i;
	GF_TrackBox *trak;

	i=0;
	while ((trak = (GF_TrackBox *)gf_list_enum(moov->trackList, &i))) {
		if (trak->Header->trackID == TrackID) {
			gf_isom_set_last_error(moov->mov, GF_BAD_PARAM);
			return 0;
		}
	}
	return 1;
}