GF_Err tssy_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TimeStampSynchronyBox *ptr = (GF_TimeStampSynchronyBox *)s;
	ISOM_DECREASE_SIZE(ptr, 1)
	gf_bs_read_int(bs, 6);
	ptr->timestamp_sync = gf_bs_read_int(bs, 2);
	return GF_OK;
}