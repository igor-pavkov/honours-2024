void gf_isom_cenc_set_saiz_saio(GF_SampleEncryptionBox *senc, GF_SampleTableBox *stbl, GF_TrackFragmentBox  *traf, u32 len, Bool saio_32bits, Bool use_multikey)
{
	u32  i;
	GF_List **child_boxes = stbl ? &stbl->child_boxes : &traf->child_boxes;
	if (!senc->cenc_saiz) {
		senc->cenc_saiz = (GF_SampleAuxiliaryInfoSizeBox *) gf_isom_box_new_parent(child_boxes, GF_ISOM_BOX_TYPE_SAIZ);
		if (!senc->cenc_saiz) return;
		//as per 3rd edition of cenc "so content SHOULD be created omitting these optional fields" ...
		senc->cenc_saiz->aux_info_type = 0;
		senc->cenc_saiz->aux_info_type_parameter = use_multikey ? 1 : 0;
		if (stbl)
			stbl_on_child_box((GF_Box*)stbl, (GF_Box *)senc->cenc_saiz, GF_FALSE);
		else
			traf_on_child_box((GF_Box*)traf, (GF_Box *)senc->cenc_saiz, GF_FALSE);
	}
	if (!senc->cenc_saio) {
		senc->cenc_saio = (GF_SampleAuxiliaryInfoOffsetBox *) gf_isom_box_new_parent(child_boxes, GF_ISOM_BOX_TYPE_SAIO);
		if (!senc->cenc_saio) return;
		//force using version 1 for saio box, it could be redundant when we use 64 bits for offset
		senc->cenc_saio->version = saio_32bits ? 0 : 1;
		//as per 3rd edition of cenc "so content SHOULD be created omitting these optional fields" ...
		senc->cenc_saio->aux_info_type = 0;
		senc->cenc_saiz->aux_info_type_parameter = use_multikey ? 1 : 0;
		senc->cenc_saio->entry_count = 1;
		if (stbl)
			stbl_on_child_box((GF_Box*)stbl, (GF_Box *)senc->cenc_saio, GF_FALSE);
		else
			traf_on_child_box((GF_Box*)traf, (GF_Box *)senc->cenc_saio, GF_FALSE);
	}

	if (!senc->cenc_saiz->sample_count || ((senc->cenc_saiz->default_sample_info_size==len) && len) ) {
		senc->cenc_saiz->sample_count ++;
		senc->cenc_saiz->default_sample_info_size = len;
	} else {
		if (senc->cenc_saiz->sample_count + 1 > senc->cenc_saiz->sample_alloc) {
			if (!senc->cenc_saiz->sample_alloc) senc->cenc_saiz->sample_alloc = senc->cenc_saiz->sample_count+1;
			else senc->cenc_saiz->sample_alloc *= 2;

			senc->cenc_saiz->sample_info_size = (u8*)gf_realloc(senc->cenc_saiz->sample_info_size, sizeof(u8)*(senc->cenc_saiz->sample_alloc));
		}

		if (senc->cenc_saiz->default_sample_info_size || (senc->cenc_saiz->sample_count==1)) {
			for (i=0; i<senc->cenc_saiz->sample_count; i++)
				senc->cenc_saiz->sample_info_size[i] = senc->cenc_saiz->default_sample_info_size;
			senc->cenc_saiz->default_sample_info_size = 0;
		}
		senc->cenc_saiz->sample_info_size[senc->cenc_saiz->sample_count] = len;
		senc->cenc_saiz->sample_count++;
	}
}