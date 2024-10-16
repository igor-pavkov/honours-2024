GF_Err tcmi_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 len;
	GF_TimeCodeMediaInformationBox *ptr = (GF_TimeCodeMediaInformationBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u16(bs, ptr->text_font);
	gf_bs_write_u16(bs, ptr->text_face);
	gf_bs_write_u16(bs, ptr->text_size);
	gf_bs_write_u16(bs, 0);
	gf_bs_write_u16(bs, ptr->text_color_red);
	gf_bs_write_u16(bs, ptr->text_color_green);
	gf_bs_write_u16(bs, ptr->text_color_blue);
	gf_bs_write_u16(bs, ptr->back_color_red);
	gf_bs_write_u16(bs, ptr->back_color_green);
	gf_bs_write_u16(bs, ptr->back_color_blue);
	len = ptr->font ? (u32) strlen(ptr->font) : 0;
	gf_bs_write_u8(bs, len);
	if (ptr->font)
		gf_bs_write_data(bs, ptr->font, len);

	return GF_OK;
}