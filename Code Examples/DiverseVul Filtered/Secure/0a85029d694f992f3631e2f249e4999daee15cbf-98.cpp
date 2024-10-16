GF_Err btrt_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_BitRateBox *ptr = (GF_BitRateBox *)s;
	ISOM_DECREASE_SIZE(ptr, 12)
	ptr->bufferSizeDB = gf_bs_read_u32(bs);
	ptr->maxBitrate = gf_bs_read_u32(bs);
	ptr->avgBitrate = gf_bs_read_u32(bs);
	return GF_OK;
}