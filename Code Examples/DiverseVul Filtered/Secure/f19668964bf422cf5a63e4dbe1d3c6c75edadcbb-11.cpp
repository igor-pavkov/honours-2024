GF_Err schm_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SchemeTypeBox *ptr = (GF_SchemeTypeBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->scheme_type);
	gf_bs_write_u32(bs, ptr->scheme_version);
	if (ptr->flags & 0x000001) {
		if (ptr->URI)
			gf_bs_write_data(bs, ptr->URI, (u32) strlen(ptr->URI)+1);
		else
			gf_bs_write_u8(bs, 0);
	}
	return GF_OK;
}