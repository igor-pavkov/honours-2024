GF_Err trun_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *) s;
	if (!s) return GF_BAD_PARAM;

#ifdef GF_ENABLE_CTRN
	if (ptr->use_ctrn)
		return ctrn_box_write(s, bs);
#endif

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->sample_count);

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRUN_DATA_OFFSET) {
		gf_bs_write_u32(bs, ptr->data_offset);
	}
	if (ptr->flags & GF_ISOM_TRUN_FIRST_FLAG) {
		gf_bs_write_u32(bs, ptr->first_sample_flags);
	}

	if (ptr->flags & (GF_ISOM_TRUN_DURATION | GF_ISOM_TRUN_SIZE | GF_ISOM_TRUN_FLAGS | GF_ISOM_TRUN_CTS_OFFSET) )  {
		for (i=0; i<ptr->nb_samples; i++) {
			GF_TrunEntry *p = &ptr->samples[i];

			if (ptr->flags & GF_ISOM_TRUN_DURATION) {
				gf_bs_write_u32(bs, p->Duration);
			}
			if (ptr->flags & GF_ISOM_TRUN_SIZE) {
				gf_bs_write_u32(bs, p->size);
			}
			//SHOULDN'T BE USED IF GF_ISOM_TRUN_FIRST_FLAG IS DEFINED
			if (ptr->flags & GF_ISOM_TRUN_FLAGS) {
				gf_bs_write_u32(bs, p->flags);
			}
			if (ptr->flags & GF_ISOM_TRUN_CTS_OFFSET) {
				if (ptr->version==0) {
					gf_bs_write_u32(bs, p->CTS_Offset);
				} else {
					gf_bs_write_u32(bs, (u32) p->CTS_Offset);
				}
			}
		}
	}

	if (ptr->sample_order) {
		u32 nb_bits = 8;
		if (ptr->sample_count>0xFFFFFF) nb_bits = 32;
		else if (ptr->sample_count>0xFFFF) nb_bits = 24;
		else if (ptr->sample_count>0xFF) nb_bits = 16;

		for (i=0; i<ptr->sample_count; i++) {
			gf_bs_write_int(bs, ptr->sample_order[i], nb_bits);
		}
	}
	return GF_OK;
}