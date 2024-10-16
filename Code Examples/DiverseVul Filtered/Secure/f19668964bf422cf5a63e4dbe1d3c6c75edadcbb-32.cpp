GF_Err iSLT_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ISMACrypSaltBox *ptr = (GF_ISMACrypSaltBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u64(bs, ptr->salt);
	return GF_OK;
}