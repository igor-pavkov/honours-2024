GF_Err unkn_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 type;
	GF_UnknownBox *ptr = (GF_UnknownBox *)s;
	if (!s) return GF_BAD_PARAM;
	type = s->type;
	ptr->type = ptr->original_4cc;
	e = gf_isom_box_write_header(s, bs);
	ptr->type = type;
	if (e) return e;

	if (ptr->sai_type) {
		if (ptr->saio_box) {
			u64 pos = gf_bs_get_position(bs);
			gf_bs_seek(bs, ptr->saio_box->offset_first_offset_field);
			if (ptr->saio_box->version)
				gf_bs_write_u64(bs, pos);
			else
				gf_bs_write_u32(bs, (u32) pos);
			gf_bs_seek(bs, pos);
		} else {
			ptr->sai_offset = gf_bs_get_position(bs);
		}
	}

	if (ptr->dataSize && ptr->data) {
		gf_bs_write_data(bs, ptr->data, ptr->dataSize);
	}
	return GF_OK;
}