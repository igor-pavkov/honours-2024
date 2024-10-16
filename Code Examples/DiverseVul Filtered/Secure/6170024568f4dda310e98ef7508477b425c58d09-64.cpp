GF_Err tmcd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TimeCodeSampleEntryBox *ptr = (GF_TimeCodeSampleEntryBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_data(bs, ptr->reserved, 6);
	gf_bs_write_u16(bs, ptr->dataReferenceIndex);

	gf_bs_write_u32(bs, 0); //reserved
	gf_bs_write_u32(bs, ptr->flags);
	gf_bs_write_u32(bs, ptr->timescale);
	gf_bs_write_u32(bs, ptr->frame_duration);
	gf_bs_write_u8(bs, ptr->frames_per_sec);
	gf_bs_write_u8(bs, 0); //reserved

	return GF_OK;
}