void gf_isom_set_default_sync_track(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *tk = gf_isom_get_track_from_file(movie, trackNumber);
	if (!tk) movie->es_id_default_sync = -1;
	else movie->es_id_default_sync = tk->Header->trackID;
}