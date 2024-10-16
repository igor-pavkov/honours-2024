GF_Err hmhd_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_HintMediaHeaderBox *ptr = (GF_HintMediaHeaderBox *)s;

	ISOM_DECREASE_SIZE(ptr, 16);
	ptr->maxPDUSize = gf_bs_read_u16(bs);
	ptr->avgPDUSize = gf_bs_read_u16(bs);
	ptr->maxBitrate = gf_bs_read_u32(bs);
	ptr->avgBitrate = gf_bs_read_u32(bs);
	ptr->slidingAverageBitrate = gf_bs_read_u32(bs);
	return GF_OK;
}