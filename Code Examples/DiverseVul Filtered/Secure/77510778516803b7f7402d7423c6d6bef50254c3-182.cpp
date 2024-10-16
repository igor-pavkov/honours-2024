GF_Err xtra_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_XtraBox *ptr = (GF_XtraBox *)s;
	u32 i, count = gf_list_count(ptr->tags);

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	for (i=0; i<count; i++) {
		GF_XtraTag *tag = gf_list_get(ptr->tags, i);
		u32 tag_size = 16;
		u32 name_len = tag->name ? (u32) strlen(tag->name) : 0;
		tag_size += name_len;
		if (tag->prop_value) {
			tag_size += 2 + tag->prop_size;
		}
		gf_bs_write_u32(bs, tag_size);
		gf_bs_write_u32(bs, name_len);
		gf_bs_write_data(bs, tag->name, name_len);
		gf_bs_write_u32(bs, tag->flags);
		gf_bs_write_u32(bs, 6 + tag->prop_size);
		gf_bs_write_u16(bs, tag->prop_type);
		gf_bs_write_data(bs, tag->prop_value, tag->prop_size);
	}
	return GF_OK;
}