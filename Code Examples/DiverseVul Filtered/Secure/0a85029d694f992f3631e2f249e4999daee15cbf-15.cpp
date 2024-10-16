GF_Err avcc_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i, count;
	GF_AVCConfigurationBox *ptr = (GF_AVCConfigurationBox *)s;

	if (ptr->config) gf_odf_avc_cfg_del(ptr->config);
	ptr->config = gf_odf_avc_cfg_new();

	ISOM_DECREASE_SIZE(ptr, 7) //7 includes the 2 counts of sps and pps

	ptr->config->configurationVersion = gf_bs_read_u8(bs);
	ptr->config->AVCProfileIndication = gf_bs_read_u8(bs);
	ptr->config->profile_compatibility = gf_bs_read_u8(bs);
	ptr->config->AVCLevelIndication = gf_bs_read_u8(bs);
	if (ptr->type==GF_ISOM_BOX_TYPE_AVCC) {
		gf_bs_read_int(bs, 6);
	} else {
		ptr->config->complete_representation = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 5);
	}
	ptr->config->nal_unit_size = 1 + gf_bs_read_int(bs, 2);
	gf_bs_read_int(bs, 3);
	count = gf_bs_read_int(bs, 5);

	for (i=0; i<count; i++) {
		ISOM_DECREASE_SIZE(ptr, 2)
		GF_NALUFFParam *sl = (GF_NALUFFParam *) gf_malloc(sizeof(GF_NALUFFParam));
		sl->size = gf_bs_read_u16(bs);
		if (!sl->size || (gf_bs_available(bs) < sl->size) || (ptr->size < sl->size) ) {
			gf_free(sl);
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("AVCC: Not enough bits to parse. Aborting.\n"));
			return GF_ISOM_INVALID_FILE;
		}
		sl->data = (char *)gf_malloc(sizeof(char) * sl->size);
		gf_bs_read_data(bs, sl->data, sl->size);
		gf_list_add(ptr->config->sequenceParameterSets, sl);
		ptr->size -= sl->size;
	}

	count = gf_bs_read_u8(bs);
	for (i=0; i<count; i++) {
		ISOM_DECREASE_SIZE(ptr, 2)
		GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_malloc(sizeof(GF_NALUFFParam));
		sl->size = gf_bs_read_u16(bs);
		if (!sl->size || (gf_bs_available(bs) < sl->size) || (ptr->size<sl->size)) {
			gf_free(sl);
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("AVCC: Not enough bits to parse. Aborting.\n"));
			return GF_ISOM_INVALID_FILE;
		}
		sl->data = (char *)gf_malloc(sizeof(char) * sl->size);
		gf_bs_read_data(bs, sl->data, sl->size);
		gf_list_add(ptr->config->pictureParameterSets, sl);
		ptr->size -= sl->size;
	}

	if (ptr->type==GF_ISOM_BOX_TYPE_AVCC) {
		if (gf_avc_is_rext_profile(ptr->config->AVCProfileIndication)) {
			if (!ptr->size) {
#ifndef GPAC_DISABLE_AV_PARSERS
				AVCState avc;
				s32 idx;
				GF_NALUFFParam *sl = (GF_NALUFFParam*)gf_list_get(ptr->config->sequenceParameterSets, 0);
				idx = sl ? gf_avc_read_sps(sl->data+1, sl->size-1, &avc, 0, NULL) : -1;

				if (idx>=0) {
					ptr->config->chroma_format = avc.sps[idx].chroma_format;
					ptr->config->luma_bit_depth = 8 + avc.sps[idx].luma_bit_depth_m8;
					ptr->config->chroma_bit_depth = 8 + avc.sps[idx].chroma_bit_depth_m8;
				}
#else
				/*set default values ...*/
				ptr->config->chroma_format = 1;
				ptr->config->luma_bit_depth = 8;
				ptr->config->chroma_bit_depth = 8;
#endif
				return GF_OK;
			}
			ISOM_DECREASE_SIZE(ptr, 4)
			gf_bs_read_int(bs, 6);
			ptr->config->chroma_format = gf_bs_read_int(bs, 2);
			gf_bs_read_int(bs, 5);
			ptr->config->luma_bit_depth = 8 + gf_bs_read_int(bs, 3);
			gf_bs_read_int(bs, 5);
			ptr->config->chroma_bit_depth = 8 + gf_bs_read_int(bs, 3);

			count = gf_bs_read_int(bs, 8);
			if (count*2 > ptr->size) {
				//ffmpeg just ignores this part while allocating bytes (filled with garbage?)
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("AVCC: invalid numOfSequenceParameterSetExt value. Skipping.\n"));
				return GF_OK;
			}
			if (count) {
				ptr->config->sequenceParameterSetExtensions = gf_list_new();
				for (i=0; i<count; i++) {
					ISOM_DECREASE_SIZE(ptr, 2)
					GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_malloc(sizeof(GF_NALUFFParam));
					sl->size = gf_bs_read_u16(bs);
					if ((gf_bs_available(bs) < sl->size) || (ptr->size<sl->size)) {
						gf_free(sl);
						GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("AVCC: Not enough bits to parse. Aborting.\n"));
						return GF_ISOM_INVALID_FILE;
					}
					sl->data = (char *)gf_malloc(sizeof(char) * sl->size);
					gf_bs_read_data(bs, sl->data, sl->size);
					gf_list_add(ptr->config->sequenceParameterSetExtensions, sl);
					ptr->size -= sl->size;
				}
			}
		}
	}
	return GF_OK;
}