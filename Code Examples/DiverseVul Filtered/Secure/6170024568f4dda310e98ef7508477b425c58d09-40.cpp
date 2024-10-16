GF_Err tcmi_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 len;
	GF_TimeCodeMediaInformationBox *ptr = (GF_TimeCodeMediaInformationBox *)s;

	ISOM_DECREASE_SIZE(s, 21);

	ptr->text_font = gf_bs_read_u16(bs);
	ptr->text_face = gf_bs_read_u16(bs);
	ptr->text_size = gf_bs_read_u16(bs);
	gf_bs_read_u16(bs);
	ptr->text_color_red = gf_bs_read_u16(bs);
	ptr->text_color_green = gf_bs_read_u16(bs);
	ptr->text_color_blue = gf_bs_read_u16(bs);
	ptr->back_color_red = gf_bs_read_u16(bs);
	ptr->back_color_green = gf_bs_read_u16(bs);
	ptr->back_color_blue = gf_bs_read_u16(bs);
	len = gf_bs_read_u8(bs);
	if (len > ptr->size)
		len = (u32) ptr->size;
	if (len) {
		ptr->font = gf_malloc(len+1);
		gf_bs_read_data(bs, ptr->font, len);
		ptr->size -= len;
		ptr->font[len]=0;
	}
	return GF_OK;
}