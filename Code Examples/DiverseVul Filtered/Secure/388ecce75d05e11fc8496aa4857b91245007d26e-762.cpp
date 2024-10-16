GF_Err tfra_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i, sap_nb_entries;
	GF_TrackFragmentRandomAccessBox *ptr = (GF_TrackFragmentRandomAccessBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->track_id);
	gf_bs_write_int(bs, 0, 26);

	gf_bs_write_int(bs, ptr->traf_bits/8 - 1, 2);
	gf_bs_write_int(bs, ptr->trun_bits/8 - 1, 2);
	gf_bs_write_int(bs, ptr->sample_bits/8 - 1, 2);

	sap_nb_entries = 0;
	for (i=0; i<ptr->nb_entries; i++) {
		GF_RandomAccessEntry *p = &ptr->entries[i];
		//no sap found, do not store
		if (p->trun_number) sap_nb_entries++;
	}

	gf_bs_write_u32(bs, sap_nb_entries);

	for (i=0; i<ptr->nb_entries; i++) {
		GF_RandomAccessEntry *p = &ptr->entries[i];
		//no sap found, do not store
		if (!p->trun_number) continue;
		if (ptr->version==1) {
			gf_bs_write_u64(bs, p->time);
			gf_bs_write_u64(bs, p->moof_offset);
		} else {
			gf_bs_write_u32(bs, (u32) p->time);
			gf_bs_write_u32(bs, (u32) p->moof_offset);
		}
		gf_bs_write_int(bs, p->traf_number, ptr->traf_bits);
		gf_bs_write_int(bs, p->trun_number, ptr->trun_bits);
		gf_bs_write_int(bs, p->sample_number, ptr->sample_bits);
	}
	return GF_OK;
}