GF_Err hdlr_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_HandlerBox *ptr = (GF_HandlerBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->reserved1);
	gf_bs_write_u32(bs, ptr->handlerType);
	gf_bs_write_data(bs, (char*)ptr->reserved2, 12);

	if (ptr->nameUTF8) {
		u32 len = (u32)strlen(ptr->nameUTF8);
		if (ptr->store_counted_string) {
			gf_bs_write_u8(bs, len);
			gf_bs_write_data(bs, ptr->nameUTF8, len);
		} else {
			gf_bs_write_data(bs, ptr->nameUTF8, len);
			gf_bs_write_u8(bs, 0);
		}
	} else {
		gf_bs_write_u8(bs, 0);
	}
	return GF_OK;
}