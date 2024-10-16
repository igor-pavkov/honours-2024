GF_Err ctrn_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i, count, flags;
	GF_TrunEntry *ent;
	GF_TrackFragmentRunBox *ctrn = (GF_TrackFragmentRunBox *) s;
	if (!s) return GF_BAD_PARAM;
	flags = ctrn->flags;
	ctrn->flags = ctrn->ctrn_flags;
	ctrn->type = GF_ISOM_BOX_TYPE_CTRN;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	ctrn->flags = flags;
	ctrn->type = GF_ISOM_BOX_TYPE_TRUN;

	gf_bs_write_u16(bs, ctrn->sample_count);
	if (ctrn->flags & GF_ISOM_TRUN_DATA_OFFSET) {
		if (ctrn->ctrn_flags & GF_ISOM_CTRN_DATAOFFSET_16) {
			gf_bs_write_u16(bs, ctrn->data_offset);
		} else {
			gf_bs_write_u32(bs, ctrn->data_offset);
		}
	}
	if (ctrn->ctso_multiplier) {
		gf_bs_write_u16(bs, ctrn->ctso_multiplier);
	}
	/*we always write first sample using first flags*/
	ent = gf_list_get(ctrn->entries, 0);
	gf_bs_write_int(bs, ent->Duration, gf_isom_ctrn_field_size_bits(ctrn->ctrn_first_dur) );
	gf_bs_write_int(bs, ent->size, gf_isom_ctrn_field_size_bits(ctrn->ctrn_first_size) );
	ctrn_write_sample_flags(bs, ent->flags, gf_isom_ctrn_field_size_bits(ctrn->ctrn_first_sample_flags) );
	ctrn_write_ctso(ctrn,bs, ent->CTS_Offset, gf_isom_ctrn_field_size_bits(ctrn->ctrn_first_ctts) );

	count = gf_list_count(ctrn->entries);
	if (ctrn->ctrn_dur) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ctrn->ctrn_dur);
		for (i=1; i<count; i++) {
			GF_TrunEntry *a_ent = gf_list_get(ctrn->entries, i);
			gf_bs_write_int(bs, a_ent->Duration, nbbits);
		}
	}
	if (ctrn->ctrn_size) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ctrn->ctrn_size);
		for (i=1; i<count; i++) {
			GF_TrunEntry *a_ent = gf_list_get(ctrn->entries, i);
			gf_bs_write_int(bs, a_ent->size, nbbits);
		}
	}
	if (ctrn->ctrn_sample_flags) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ctrn->ctrn_sample_flags);
		for (i=1; i<count; i++) {
			GF_TrunEntry *a_ent = gf_list_get(ctrn->entries, i);
			ctrn_write_sample_flags(bs, a_ent->flags, nbbits);
		}
	}
	if (ctrn->ctrn_ctts) {
		u32 nbbits = gf_isom_ctrn_field_size_bits(ctrn->ctrn_ctts);
		for (i=1; i<count; i++) {
			GF_TrunEntry *a_ent = gf_list_get(ctrn->entries, i);
			ctrn_write_ctso(ctrn, bs, a_ent->CTS_Offset, nbbits);
		}
	}

	return GF_OK;
}