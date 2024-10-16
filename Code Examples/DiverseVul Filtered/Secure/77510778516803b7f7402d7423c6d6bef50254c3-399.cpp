GF_Err sbgp_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_Err e;
	GF_SampleGroupBox *p = (GF_SampleGroupBox*)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, p->grouping_type);
	if (p->version==1)
		gf_bs_write_u32(bs, p->grouping_type_parameter);

	gf_bs_write_u32(bs, p->entry_count);
	for (i = 0; i<p->entry_count; i++ ) {
		gf_bs_write_u32(bs, p->sample_entries[i].sample_count);
		gf_bs_write_u32(bs, p->sample_entries[i].group_description_index);
	}
	return GF_OK;
}