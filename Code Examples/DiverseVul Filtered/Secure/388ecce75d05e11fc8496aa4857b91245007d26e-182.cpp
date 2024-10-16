GF_Err rssr_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ReceivedSsrcBox *ptr = (GF_ReceivedSsrcBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->ssrc);
	return GF_OK;
}