GF_ISOTrackID gf_isom_get_highest_track_in_scalable_segment(GF_ISOFile *movie, u32 for_base_track)
{
#ifdef	GPAC_DISABLE_ISOM_FRAGMENTS
	return 0;
#else
	s32 max_ref;
	u32 i, j;
	GF_ISOTrackID track_id;

	max_ref = 0;
	track_id = 0;
	for (i=0; i<gf_list_count(movie->moov->trackList); i++) {
		s32 ref;
		u32 ref_type = GF_ISOM_REF_SCAL;
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(movie->moov->trackList, i);
		if (! trak->present_in_scalable_segment) continue;

		ref = gf_isom_get_reference_count(movie, i+1, ref_type);
		if (ref<=0) {
			//handle implicit reconstruction for LHE1/LHV1, check sbas track ref
			u32 subtype = gf_isom_get_media_subtype(movie, i+1, 1);
			switch (subtype) {
			case GF_ISOM_SUBTYPE_LHE1:
			case GF_ISOM_SUBTYPE_LHV1:
				ref = gf_isom_get_reference_count(movie, i+1, GF_ISOM_REF_BASE);
				if (ref<=0) continue;
				break;
			default:
				continue;
			}
		}
		if (ref<=max_ref) continue;

		for (j=0; j< (u32) ref; j++) {
			u32 on_track=0;
			gf_isom_get_reference(movie, i+1, GF_ISOM_REF_BASE, j+1, &on_track);
			if (on_track==for_base_track) {
				max_ref = ref;
				track_id = trak->Header->trackID;
			}
		}
	}
	return track_id;
#endif
}