GF_Err ihdr_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_J2KImageHeaderBox *ptr = (GF_J2KImageHeaderBox *) s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->height);
	gf_bs_write_u32(bs, ptr->width);
	gf_bs_write_u16(bs, ptr->nb_comp);
	gf_bs_write_u8(bs, ptr->bpc);
	gf_bs_write_u8(bs, ptr->Comp);
	gf_bs_write_u8(bs, ptr->UnkC);
	gf_bs_write_u8(bs, ptr->IPR);
	return GF_OK;
}