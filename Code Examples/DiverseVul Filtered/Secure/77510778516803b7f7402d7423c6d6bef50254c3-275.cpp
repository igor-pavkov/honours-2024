GF_Err tfra_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_RandomAccessEntry *p = 0;
	GF_TrackFragmentRandomAccessBox *ptr = (GF_TrackFragmentRandomAccessBox *)s;

	ISOM_DECREASE_SIZE(ptr, 12);

	ptr->track_id = gf_bs_read_u32(bs);
	if (gf_bs_read_int(bs, 26) != 0)
		return GF_ISOM_INVALID_FILE;

	ptr->traf_bits = (gf_bs_read_int(bs, 2) + 1) * 8;
	ptr->trun_bits = (gf_bs_read_int(bs, 2) + 1) * 8;
	ptr->sample_bits = (gf_bs_read_int(bs, 2) + 1) * 8;

	ptr->nb_entries = gf_bs_read_u32(bs);

	if (ptr->version == 1) {
		if (ptr->nb_entries > ptr->size / (16+(ptr->traf_bits+ptr->trun_bits+ptr->sample_bits)/8)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in traf\n", ptr->nb_entries));
			return GF_ISOM_INVALID_FILE;
		}
	} else {
		if (ptr->nb_entries > ptr->size / (8+(ptr->traf_bits+ptr->trun_bits+ptr->sample_bits)/8)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in traf\n", ptr->nb_entries));
			return GF_ISOM_INVALID_FILE;
		}
	}

	if (ptr->nb_entries) {
		if ((u64)ptr->nb_entries > (u64)SIZE_MAX/sizeof(GF_RandomAccessEntry)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in traf\n", ptr->nb_entries));
			return GF_ISOM_INVALID_FILE;
		}
		p = (GF_RandomAccessEntry *) gf_malloc(sizeof(GF_RandomAccessEntry) * ptr->nb_entries);
		if (!p) return GF_OUT_OF_MEM;
	}

	ptr->entries = p;

	for (i=0; i<ptr->nb_entries; i++) {
		memset(p, 0, sizeof(GF_RandomAccessEntry));

		if (ptr->version == 1) {
			p->time = gf_bs_read_u64(bs);
			p->moof_offset = gf_bs_read_u64(bs);
		} else {
			p->time = gf_bs_read_u32(bs);
			p->moof_offset = gf_bs_read_u32(bs);
		}
		p->traf_number = gf_bs_read_int(bs, ptr->traf_bits);
		p->trun_number = gf_bs_read_int(bs, ptr->trun_bits);
		p->sample_number = gf_bs_read_int(bs, ptr->sample_bits);

		++p;
	}

	return GF_OK;
}