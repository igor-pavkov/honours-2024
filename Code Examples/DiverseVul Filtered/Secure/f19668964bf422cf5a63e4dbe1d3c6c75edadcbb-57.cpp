GF_Err store_senc_info(GF_SampleEncryptionBox *ptr, GF_BitStream *bs)
{
	GF_Err e;
	u64 pos, new_pos;
	if (!ptr->cenc_saio) return GF_OK;

	pos = gf_bs_get_position(bs);
	if (pos>0xFFFFFFFFULL) {
		if (ptr->cenc_saio && !ptr->cenc_saio->version) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] saio offset larger than 32-bits but box version 0 enforced. Retry without \"saio32\" option\n"));
			return GF_BAD_PARAM;
		}
	}
	e = gf_bs_seek(bs, ptr->cenc_saio->offset_first_offset_field);
	if (e) return e;
	//force using version 1 for saio box i.e offset has 64 bits
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	if (ptr->traf) {
		new_pos = pos - ptr->traf->moof_start_in_bs;
	} else
#endif
	{
		new_pos = pos;
	}

	if (ptr->cenc_saio->offsets) {
		u32 i;
		u64 old_offset = ptr->cenc_saio->offsets[0];
		for (i=0; i<ptr->cenc_saio->entry_count; i++) {
			if (ptr->cenc_saio->version) {
				gf_bs_write_u64(bs, new_pos + ptr->cenc_saio->offsets[i] - old_offset);
			} else {
				gf_bs_write_u32(bs, (u32) (new_pos + ptr->cenc_saio->offsets[i] - old_offset));
			}
			ptr->cenc_saio->offsets[i] = new_pos + ptr->cenc_saio->offsets[i] - old_offset;
		}
	} else {
		if (ptr->cenc_saio->version) {
			gf_bs_write_u64(bs, new_pos);
		} else {
			gf_bs_write_u32(bs, (u32) new_pos);
		}
	}

	return gf_bs_seek(bs, pos);
}