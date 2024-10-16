GF_Err rssr_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ReceivedSsrcBox *ptr = (GF_ReceivedSsrcBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->ssrc = gf_bs_read_u32(bs);
	return GF_OK;
}