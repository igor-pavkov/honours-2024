GF_Err chpl_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 count, i;
	GF_ChapterListBox *ptr = (GF_ChapterListBox *) s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	count = gf_list_count(ptr->list);
	gf_bs_write_u32(bs, 0);
	gf_bs_write_u8(bs, count);
	for (i=0; i<count; i++) {
		u32 len;
		GF_ChapterEntry *ce = (GF_ChapterEntry *)gf_list_get(ptr->list, i);
		gf_bs_write_u64(bs, ce->start_time);
		if (ce->name) {
			len = (u32) strlen(ce->name);
			if (len>255) len = 255;
			gf_bs_write_u8(bs, len);
			gf_bs_write_data(bs, ce->name, len);
		} else {
			gf_bs_write_u8(bs, 0);
		}
	}
	return GF_OK;
}