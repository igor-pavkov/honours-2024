GF_Err nump_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_NUMPBox *ptr = (GF_NUMPBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u64(bs, ptr->nbPackets);
	return GF_OK;
}