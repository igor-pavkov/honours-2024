GF_Err gmin_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_GenericMediaHeaderInfoBox *ptr = (GF_GenericMediaHeaderInfoBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, ptr->graphics_mode);
	gf_bs_write_u16(bs, ptr->op_color_red);
	gf_bs_write_u16(bs, ptr->op_color_green);
	gf_bs_write_u16(bs, ptr->op_color_blue);
	gf_bs_write_u16(bs, ptr->balance);
	gf_bs_write_u16(bs, ptr->reserved);
	return GF_OK;
}