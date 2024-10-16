GF_Err gf_isom_get_fragmented_samples_info(GF_ISOFile *movie, GF_ISOTrackID trackID, u32 *nb_samples, u64 *duration)
{
	u32 i=0;
	u32 k, l;
	GF_MovieFragmentBox *moof;
	GF_TrackFragmentBox *traf;

	*nb_samples = 0;
	*duration = 0;
	while ((moof=(GF_MovieFragmentBox*)gf_list_enum(movie->TopBoxes, &i))) {
		u32 j=0;
		if (moof->type!=GF_ISOM_BOX_TYPE_MOOF) continue;

		while ((traf=(GF_TrackFragmentBox*)gf_list_enum( moof->TrackList, &j))) {
			u64 def_duration, samp_dur=0;

			if (traf->tfhd->trackID != trackID)
				continue;

			def_duration = 0;
			if (traf->tfhd->flags & GF_ISOM_TRAF_SAMPLE_DUR) def_duration = traf->tfhd->def_sample_duration;
			else if (traf->trex) def_duration = traf->trex->def_sample_duration;

			for (k=0; k<gf_list_count(traf->TrackRuns); k++) {
				GF_TrackFragmentRunBox *trun = (GF_TrackFragmentRunBox*)gf_list_get(traf->TrackRuns, k);
				*nb_samples += trun->sample_count;

				for (l=0; l<trun->nb_samples; l++) {
					GF_TrunEntry *ent = &trun->samples[l];

					samp_dur = def_duration;
					if (trun->flags & GF_ISOM_TRUN_DURATION) samp_dur = ent->Duration;
					if (trun->nb_samples == trun->sample_count)
						*duration += samp_dur;
				}
				if (trun->nb_samples != trun->sample_count)
					*duration += samp_dur * trun->sample_count;
			}
		}
	}
	return GF_OK;
}