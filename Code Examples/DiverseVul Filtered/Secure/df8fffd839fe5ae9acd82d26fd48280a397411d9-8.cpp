GF_Err GetESDForTime(GF_MovieBox *moov, GF_ISOTrackID trackID, u64 CTS, GF_ESD **outESD)
{
	GF_Err e;
	u32 sampleDescIndex;
	GF_TrackBox *trak;

	trak = gf_isom_get_track(moov, gf_isom_get_tracknum_from_id(moov, trackID));
	if (!trak) return GF_ISOM_INVALID_FILE;

	e = Media_GetSampleDescIndex(trak->Media, CTS, &sampleDescIndex );
	if (e) return e;
	return GetESD(moov, trackID, sampleDescIndex, outESD);
}