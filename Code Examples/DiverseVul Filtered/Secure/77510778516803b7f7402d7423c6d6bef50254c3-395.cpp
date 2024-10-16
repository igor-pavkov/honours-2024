GF_Err sidx_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_SegmentIndexBox *ptr = (GF_SegmentIndexBox*) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->reference_ID);
	gf_bs_write_u32(bs, ptr->timescale);
	if (ptr->version==0) {
		gf_bs_write_u32(bs, (u32) ptr->earliest_presentation_time);
		gf_bs_write_u32(bs, (u32) ptr->first_offset);
	} else {
		gf_bs_write_u64(bs, ptr->earliest_presentation_time);
		gf_bs_write_u64(bs, ptr->first_offset);
	}
	gf_bs_write_u16(bs, 0);
	gf_bs_write_u16(bs, ptr->nb_refs);
	for (i=0; i<ptr->nb_refs; i++ ) {
		gf_bs_write_int(bs, ptr->refs[i].reference_type, 1);
		gf_bs_write_int(bs, ptr->refs[i].reference_size, 31);
		gf_bs_write_u32(bs, ptr->refs[i].subsegment_duration);
		gf_bs_write_int(bs, ptr->refs[i].starts_with_SAP, 1);
		gf_bs_write_int(bs, ptr->refs[i].SAP_type, 3);
		gf_bs_write_int(bs, ptr->refs[i].SAP_delta_time, 28);
	}
	return GF_OK;
}