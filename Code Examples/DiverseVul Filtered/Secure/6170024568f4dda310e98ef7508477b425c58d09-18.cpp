GF_Err chrm_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_ChromaInfoBox *ptr = (GF_ChromaInfoBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u16(bs, ptr->chroma);
	return GF_OK;
}