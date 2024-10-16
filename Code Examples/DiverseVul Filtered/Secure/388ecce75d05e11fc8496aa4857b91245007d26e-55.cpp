GF_Err tfra_box_size(GF_Box *s)
{
	u32 i;
	GF_TrackFragmentRandomAccessBox *ptr = (GF_TrackFragmentRandomAccessBox *)s;
	ptr->size += 12;

	for (i=0; i<ptr->nb_entries; i++) {
		GF_RandomAccessEntry *p = &ptr->entries[i];
		//no sap found, do not store
		if (!p->trun_number) continue;
		ptr->size +=  ((ptr->version==1) ? 16 : 8 ) + ptr->traf_bits/8 + ptr->trun_bits/8 + ptr->sample_bits/8;
	}
	return GF_OK;
}