GF_Err gmin_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_GenericMediaHeaderInfoBox *ptr = (GF_GenericMediaHeaderInfoBox *)s;
	ISOM_DECREASE_SIZE(ptr, 12);
	ptr->graphics_mode = gf_bs_read_u16(bs);
	ptr->op_color_red = gf_bs_read_u16(bs);
	ptr->op_color_green = gf_bs_read_u16(bs);
	ptr->op_color_blue = gf_bs_read_u16(bs);
	ptr->balance = gf_bs_read_u16(bs);
	ptr->reserved = gf_bs_read_u16(bs);
	return GF_OK;
}