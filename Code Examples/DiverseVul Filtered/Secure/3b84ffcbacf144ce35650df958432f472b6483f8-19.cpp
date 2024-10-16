GF_Err gf_isom_cenc_merge_saiz_saio(GF_SampleEncryptionBox *senc, GF_SampleTableBox *stbl, u64 offset, u32 len)
{
	u32  i;
	assert(stbl);
	if (!senc->cenc_saiz) {
		senc->cenc_saiz = (GF_SampleAuxiliaryInfoSizeBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_SAIZ);
		if (!senc->cenc_saiz) return GF_OUT_OF_MEM;
		senc->cenc_saiz->aux_info_type = GF_ISOM_CENC_SCHEME;
		senc->cenc_saiz->aux_info_type_parameter = 0;
		stbl_on_child_box((GF_Box*)stbl, (GF_Box *)senc->cenc_saiz, GF_FALSE);
	}
	if (!senc->cenc_saio) {
		senc->cenc_saio = (GF_SampleAuxiliaryInfoOffsetBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_SAIO);
		if (!senc->cenc_saio) return GF_OUT_OF_MEM;
		//force using version 1 for saio box, it could be redundant when we use 64 bits for offset
		senc->cenc_saio->version = 1;
		senc->cenc_saio->aux_info_type = GF_ISOM_CENC_SCHEME;
		senc->cenc_saio->aux_info_type_parameter = 0;
		stbl_on_child_box((GF_Box*)stbl, (GF_Box *)senc->cenc_saio, GF_FALSE);
	}

	if (!senc->cenc_saiz->sample_count || (!senc->cenc_saiz->sample_alloc && (senc->cenc_saiz->default_sample_info_size==len))) {
		senc->cenc_saiz->sample_count ++;
		senc->cenc_saiz->default_sample_info_size = len;
	} else {
		if (senc->cenc_saiz->sample_count + 1 > senc->cenc_saiz->sample_alloc) {
			if (!senc->cenc_saiz->sample_alloc) senc->cenc_saiz->sample_alloc = senc->cenc_saiz->sample_count + 1;
			else senc->cenc_saiz->sample_alloc *= 2;
			senc->cenc_saiz->sample_info_size = (u8*)gf_realloc(senc->cenc_saiz->sample_info_size, sizeof(u8)*(senc->cenc_saiz->sample_alloc));
			if (!senc->cenc_saiz->sample_info_size) return GF_OUT_OF_MEM;
		}

		if (senc->cenc_saiz->default_sample_info_size) {
			for (i=0; i<senc->cenc_saiz->sample_count; i++)
				senc->cenc_saiz->sample_info_size[i] = senc->cenc_saiz->default_sample_info_size;
			senc->cenc_saiz->default_sample_info_size = 0;
		}
		senc->cenc_saiz->sample_info_size[senc->cenc_saiz->sample_count] = len;
		senc->cenc_saiz->sample_count++;
	}

	if (!senc->cenc_saio->entry_count) {
		senc->cenc_saio->offsets = (u64 *)gf_malloc(sizeof(u64));
		if (!senc->cenc_saio->offsets) return GF_OUT_OF_MEM;
		senc->cenc_saio->offsets[0] = offset;
		senc->cenc_saio->entry_count ++;
		senc->cenc_saio->entry_alloc = 1;
	} else {
		if (senc->cenc_saio->entry_count >= senc->cenc_saio->entry_alloc) {
			senc->cenc_saio->entry_alloc += 50;
			senc->cenc_saio->offsets = (u64*)gf_realloc(senc->cenc_saio->offsets, sizeof(u64)*(senc->cenc_saio->entry_alloc));
			if (!senc->cenc_saio->offsets) return GF_OUT_OF_MEM;
		}
		senc->cenc_saio->offsets[senc->cenc_saio->entry_count] = offset;
		senc->cenc_saio->entry_count++;
	}
	if (offset > 0xFFFFFFFFUL)
		senc->cenc_saio->version=1;
	return GF_OK;
}