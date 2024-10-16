GF_Err free_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_FreeSpaceBox *ptr = (GF_FreeSpaceBox *)s;
	if (ptr->original_4cc) {
		u32 t = s->type;
		s->type=ptr->original_4cc;
		e = gf_isom_box_write_header(s, bs);
		s->type=t;
	} else {
		e = gf_isom_box_write_header(s, bs);
	}
	if (e) return e;
	if (ptr->dataSize)	{
		if (ptr->data) {
			gf_bs_write_data(bs, ptr->data, ptr->dataSize);
		} else {
			u32 i = 0;
			while (i<ptr->dataSize) {
				gf_bs_write_u8(bs, 0);
				i++;
			}
		}
	}
	return GF_OK;
}