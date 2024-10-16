GF_Err gama_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_GamaInfoBox *ptr = (GF_GamaInfoBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->gama);
	return GF_OK;
}