GF_TrunEntry *traf_get_sample_entry(GF_TrackFragmentBox *traf, u32 sample_index)
{
	u32 i, idx;
	GF_TrackFragmentRunBox *trun;
	idx=0;
	i=0;
	while ((trun = (GF_TrackFragmentRunBox *)gf_list_enum(traf->TrackRuns, &i))) {
		u32 j;
		for (j=0; j<trun->sample_count; j++) {
			GF_TrunEntry *ent = gf_list_get(trun->entries, j);
			if (idx==sample_index) return ent;
			if (ent->nb_pack>1) {
				if (idx < sample_index + ent->nb_pack)
					return ent;
				idx += ent->nb_pack;
			} else {
				idx++;
			}
		}
	}
	return NULL;
}