GF_Err hmhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_HintMediaHeaderBox *ptr = (GF_HintMediaHeaderBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u16(bs, ptr->maxPDUSize);
	gf_bs_write_u16(bs, ptr->avgPDUSize);
	gf_bs_write_u32(bs, ptr->maxBitrate);
	gf_bs_write_u32(bs, ptr->avgBitrate);
	gf_bs_write_u32(bs, ptr->slidingAverageBitrate);
	return GF_OK;
}