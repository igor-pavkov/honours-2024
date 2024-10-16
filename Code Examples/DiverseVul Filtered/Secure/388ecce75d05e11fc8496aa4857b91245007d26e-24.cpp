GF_Err tssy_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TimeStampSynchronyBox *ptr = (GF_TimeStampSynchronyBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, 0, 6);
	gf_bs_write_int(bs, ptr->timestamp_sync, 2);
	return GF_OK;
}