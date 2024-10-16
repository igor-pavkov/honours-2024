GF_Err btrt_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_BitRateBox *ptr = (GF_BitRateBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->bufferSizeDB);
	gf_bs_write_u32(bs, ptr->maxBitrate);
	gf_bs_write_u32(bs, ptr->avgBitrate);
	return GF_OK;
}