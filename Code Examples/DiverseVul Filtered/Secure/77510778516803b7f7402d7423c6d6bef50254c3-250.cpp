GF_Err pcrb_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_PcrInfoBox *ptr = (GF_PcrInfoBox*) s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->subsegment_count);

	for (i=0; i<ptr->subsegment_count; i++ ) {
		u32 data1 = (u32) (ptr->pcr_values[i] >> 10);
		u16 data2 = (u16) (ptr->pcr_values[i] << 6);

		gf_bs_write_u32(bs, data1);
		gf_bs_write_u16(bs, data2);
	}
	return GF_OK;
}