GF_Err saio_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SampleAuxiliaryInfoOffsetBox *ptr = (GF_SampleAuxiliaryInfoOffsetBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	if (ptr->flags & 1) {
		gf_bs_write_u32(bs, ptr->aux_info_type);
		gf_bs_write_u32(bs, ptr->aux_info_type_parameter);
	}


	gf_bs_write_u32(bs, ptr->entry_count);
	if (ptr->entry_count) {
		u32 i;
		if (ptr->sai_data) {
			if (ptr->sai_data->sai_offset) {
				if (ptr->version==0) {
					gf_bs_write_u32(bs, (u32) ptr->sai_data->sai_offset);
				} else {
					gf_bs_write_u64(bs, ptr->sai_data->sai_offset);
				}
				return GF_OK;
			}
			ptr->sai_data->saio_box = ptr;
		}

		//store position in bitstream before writing data - offsets can be NULL if a single offset is rewritten later on (cf senc_box_write)
		ptr->offset_first_offset_field = gf_bs_get_position(bs);
		if (ptr->version==0) {
			if (!ptr->offsets) {
				gf_bs_write_u32(bs, 0);
			} else {
				for (i=0; i<ptr->entry_count; i++)
					gf_bs_write_u32(bs, (u32) ptr->offsets[i]);
			}
		} else {
			if (!ptr->offsets) {
				gf_bs_write_u64(bs, 0);
			} else {
				for (i=0; i<ptr->entry_count; i++)
					gf_bs_write_u64(bs, ptr->offsets[i]);
			}
		}
	}
	return GF_OK;
}