GF_Err avcc_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 i, count;
	GF_Err e;
	GF_AVCConfigurationBox *ptr = (GF_AVCConfigurationBox *) s;
	if (!s) return GF_BAD_PARAM;
	if (!ptr->config) return GF_OK;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u8(bs, ptr->config->configurationVersion);
	gf_bs_write_u8(bs, ptr->config->AVCProfileIndication);
	gf_bs_write_u8(bs, ptr->config->profile_compatibility);
	gf_bs_write_u8(bs, ptr->config->AVCLevelIndication);
	if (ptr->type==GF_ISOM_BOX_TYPE_AVCC) {
		gf_bs_write_int(bs, 0x3F, 6);
	} else {
		gf_bs_write_int(bs, ptr->config->complete_representation, 1);
		gf_bs_write_int(bs, 0x1F, 5);
	}
	gf_bs_write_int(bs, ptr->config->nal_unit_size - 1, 2);
	gf_bs_write_int(bs, 0x7, 3);
	count = gf_list_count(ptr->config->sequenceParameterSets);
	gf_bs_write_int(bs, count, 5);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *) gf_list_get(ptr->config->sequenceParameterSets, i);
		gf_bs_write_u16(bs, sl->size);
		gf_bs_write_data(bs, sl->data, sl->size);
	}

	count = gf_list_count(ptr->config->pictureParameterSets);
	gf_bs_write_u8(bs, count);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *) gf_list_get(ptr->config->pictureParameterSets, i);
		gf_bs_write_u16(bs, sl->size);
		gf_bs_write_data(bs, sl->data, sl->size);
	}


	if (ptr->type==GF_ISOM_BOX_TYPE_AVCC) {
		if (gf_avc_is_rext_profile(ptr->config->AVCProfileIndication)) {
			gf_bs_write_int(bs, 0xFF, 6);
			gf_bs_write_int(bs, ptr->config->chroma_format, 2);
			gf_bs_write_int(bs, 0xFF, 5);
			gf_bs_write_int(bs, ptr->config->luma_bit_depth - 8, 3);
			gf_bs_write_int(bs, 0xFF, 5);
			gf_bs_write_int(bs, ptr->config->chroma_bit_depth - 8, 3);

			count = ptr->config->sequenceParameterSetExtensions ? gf_list_count(ptr->config->sequenceParameterSetExtensions) : 0;
			gf_bs_write_u8(bs, count);
			for (i=0; i<count; i++) {
				GF_NALUFFParam *sl = (GF_NALUFFParam *) gf_list_get(ptr->config->sequenceParameterSetExtensions, i);
				gf_bs_write_u16(bs, sl->size);
				gf_bs_write_data(bs, sl->data, sl->size);
			}
		}
	}
	return GF_OK;
}