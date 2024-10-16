static void FixTrackID(GF_ISOFile *mov)
{
	if (!mov->moov) return;

	if (gf_list_count(mov->moov->trackList) == 1 && gf_list_count(mov->moof->TrackList) == 1) {
		GF_TrackFragmentBox *traf = (GF_TrackFragmentBox*)gf_list_get(mov->moof->TrackList, 0);
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(mov->moov->trackList, 0);
		if (!traf->tfhd || !trak->Header) return;
		if ((traf->tfhd->trackID != trak->Header->trackID)) {
			if (!mov->is_smooth) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Warning: trackID of MOOF/TRAF(%u) is not the same as MOOV/TRAK(%u). Trying to fix.\n", traf->tfhd->trackID, trak->Header->trackID));
			} else {
				trak->Header->trackID = traf->tfhd->trackID;
			}
			traf->tfhd->trackID = trak->Header->trackID;
		}
	}
}