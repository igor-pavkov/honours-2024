static GF_Err ctrn_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i, count, flags, first_idx=0;
	Bool inherit_dur, inherit_size, inherit_flags, inherit_ctso;
	GF_TrunEntry *ent;
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *)s;
	flags = ptr->flags;
	ptr->ctrn_flags = flags;
	ptr->flags = 0;

	ptr->sample_count = gf_bs_read_u16(bs);
	ISOM_DECREASE_SIZE(ptr, 2);

	if (flags & GF_ISOM_TRUN_DATA_OFFSET) {
		if (flags & GF_ISOM_CTRN_DATAOFFSET_16) {
			ptr->data_offset = gf_bs_read_u16(bs);
			ISOM_DECREASE_SIZE(ptr, 2);
		} else {
			ptr->data_offset = gf_bs_read_u32(bs);
			ISOM_DECREASE_SIZE(ptr, 4);
		}
		ptr->flags |= GF_ISOM_TRUN_DATA_OFFSET;
	}
	if (flags & GF_ISOM_CTRN_CTSO_MULTIPLIER) {
		ptr->ctso_multiplier = gf_bs_read_u16(bs);
		ISOM_DECREASE_SIZE(ptr, 2);
	}
	/*no sample dur/sample_flag/size/ctso for first or following, create a pack sample */
	if (! (flags & 0x00FFFF00)) {
		GF_SAFEALLOC(ent, GF_TrunEntry);
		if (!ent) return GF_OUT_OF_MEM;
		ent->nb_pack = ptr->sample_count;
		gf_list_add(ptr->entries, ent);
		return GF_OK;
	}
	/*allocate all entries*/
	for (i=0; i<ptr->sample_count; i++) {
		GF_SAFEALLOC(ent, GF_TrunEntry);
		if (!ent) return GF_OUT_OF_MEM;
		gf_list_add(ptr->entries, ent);
	}
	//unpack flags
	ptr->ctrn_first_dur = (flags>>22) & 0x3;
	ptr->ctrn_first_size = (flags>>20) & 0x3;
	ptr->ctrn_first_sample_flags = (flags>>18) & 0x3;
	ptr->ctrn_first_ctts = (flags>>16) & 0x3;
	ptr->ctrn_dur = (flags>>14) & 0x3;
	ptr->ctrn_size = (flags>>12) & 0x3;
	ptr->ctrn_sample_flags = (flags>>10) & 0x3;
	ptr->ctrn_ctts = (flags>>8) & 0x3;

	inherit_dur = flags & GF_ISOM_CTRN_INHERIT_DUR;
	inherit_size = flags & GF_ISOM_CTRN_INHERIT_SIZE;
	inherit_flags = flags & GF_ISOM_CTRN_INHERIT_FLAGS;
	inherit_ctso = flags & GF_ISOM_CTRN_INHERIT_CTSO;

	if (flags & GF_ISOM_CTRN_FIRST_SAMPLE) {
		ent = gf_list_get(ptr->entries, 0);
		first_idx = 1;
		if (!inherit_dur && ptr->ctrn_first_dur) {
			ent->Duration = gf_bs_read_int(bs, gf_isom_ctrn_field_size_bits(ptr->ctrn_first_dur) );
			ISOM_DECREASE_SIZE(ptr, ctrn_field_size(ptr->ctrn_first_dur) );
		}
		if (!inherit_size && ptr->ctrn_first_size) {
			ent->size = gf_bs_read_int(bs, gf_isom_ctrn_field_size_bits(ptr->ctrn_first_size) );
			ISOM_DECREASE_SIZE(ptr, ctrn_field_size(ptr->ctrn_first_size) );
		}
		if (!inherit_flags && ptr->ctrn_first_sample_flags) {
			ent->flags = ctrn_read_flags(bs, gf_isom_ctrn_field_size_bits(ptr->ctrn_first_sample_flags) );
			ISOM_DECREASE_SIZE(ptr, ctrn_field_size(ptr->ctrn_first_sample_flags) );
		}
		if (!inherit_ctso && ptr->ctrn_first_ctts) {
			ent->CTS_Offset = gf_bs_read_int(bs, gf_isom_ctrn_field_size_bits(ptr->ctrn_first_ctts) );
			ISOM_DECREASE_SIZE(ptr, ctrn_field_size(ptr->ctrn_first_ctts) );
			if (ptr->ctso_multiplier)
				ent->CTS_Offset *= (s32) ptr->ctso_multiplier;
		}
	}
	count = ptr->sample_count - first_idx;
	if (!inherit_dur && ptr->ctrn_dur) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ptr->ctrn_dur);
		ISOM_DECREASE_SIZE(ptr, count * nbbits / 8);
		for (i=first_idx; i<ptr->sample_count; i++) {
			ent = gf_list_get(ptr->entries, i);
			ent->Duration = gf_bs_read_int(bs, nbbits);
		}
	}
	if (!inherit_size && ptr->ctrn_size) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ptr->ctrn_size);
		ISOM_DECREASE_SIZE(ptr, count * nbbits / 8);
		for (i=first_idx; i<ptr->sample_count; i++) {
			ent = gf_list_get(ptr->entries, i);
			ent->size = gf_bs_read_int(bs, nbbits);
		}
	}
	if (!inherit_flags && ptr->ctrn_sample_flags) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ptr->ctrn_sample_flags);
		ISOM_DECREASE_SIZE(ptr, count * nbbits / 8);
		for (i=first_idx; i<ptr->sample_count; i++) {
			ent = gf_list_get(ptr->entries, i);
			ent->flags = ctrn_read_flags(bs, nbbits);
		}
	}
	if (!inherit_ctso && ptr->ctrn_ctts) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ptr->ctrn_ctts);
		ISOM_DECREASE_SIZE(ptr, count * nbbits / 8);
		for (i=first_idx; i<ptr->sample_count; i++) {
			ent = gf_list_get(ptr->entries, i);
			ent->CTS_Offset = gf_bs_read_int(bs, nbbits);
			if (ptr->ctso_multiplier)
				ent->CTS_Offset *= (s32) ptr->ctso_multiplier;
		}
	}

	return GF_OK;
}