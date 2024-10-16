static GF_ISOSAPType is_sample_idr(GF_MediaBox *mdia, GF_ISOSample *sample, GF_MPEGVisualSampleEntryBox *entry)
{
	Bool is_hevc = GF_FALSE;
	u32 nalu_size_field = 0;
	if (entry->avc_config && entry->avc_config->config) nalu_size_field = entry->avc_config->config->nal_unit_size;
	else if (entry->svc_config && entry->svc_config->config) nalu_size_field = entry->svc_config->config->nal_unit_size;
	else if (entry->mvc_config && entry->mvc_config->config) nalu_size_field = entry->mvc_config->config->nal_unit_size;
	else if (entry->hevc_config && entry->hevc_config->config) {
		nalu_size_field = entry->hevc_config->config->nal_unit_size;
		is_hevc = GF_TRUE;
	}
	else if (entry->lhvc_config && entry->lhvc_config->config) {
		nalu_size_field = entry->lhvc_config->config->nal_unit_size;
		is_hevc = GF_TRUE;
	}
	if (!nalu_size_field) return RAP_NO;

	if (!mdia->nalu_parser)
		mdia->nalu_parser = gf_bs_new(sample->data, sample->dataLength, GF_BITSTREAM_READ);
	else
		gf_bs_reassign_buffer(mdia->nalu_parser, sample->data, sample->dataLength);

	if (!mdia->nalu_parser) return RAP_NO;

	while (gf_bs_available(mdia->nalu_parser)) {
		u8 nal_type;
		u32 size = gf_bs_read_int(mdia->nalu_parser, 8*nalu_size_field);

		if (is_hevc) {
#ifndef GPAC_DISABLE_HEVC
			u16 nal_hdr = gf_bs_read_u16(mdia->nalu_parser);
			nal_type = (nal_hdr&0x7E00) >> 9;

			switch (nal_type) {
			case GF_HEVC_NALU_SLICE_CRA:
				return SAP_TYPE_3;
			case GF_HEVC_NALU_SLICE_IDR_N_LP:
			case GF_HEVC_NALU_SLICE_BLA_N_LP:
				return SAP_TYPE_1;
			case GF_HEVC_NALU_SLICE_IDR_W_DLP:
			case GF_HEVC_NALU_SLICE_BLA_W_DLP:
			case GF_HEVC_NALU_SLICE_BLA_W_LP:
				return SAP_TYPE_2;
			case GF_HEVC_NALU_ACCESS_UNIT:
			case GF_HEVC_NALU_FILLER_DATA:
			case GF_HEVC_NALU_SEI_PREFIX:
			case GF_HEVC_NALU_VID_PARAM:
			case GF_HEVC_NALU_SEQ_PARAM:
			case GF_HEVC_NALU_PIC_PARAM:
				break;
			default:
				return RAP_NO;
			}
			gf_bs_skip_bytes(mdia->nalu_parser, size - 2);
#endif
		} else {
			u8 nal_hdr = gf_bs_read_u8(mdia->nalu_parser);
			nal_type = nal_hdr & 0x1F;

			if (nal_type==GF_AVC_NALU_IDR_SLICE) return SAP_TYPE_1;
			if (nal_type<GF_AVC_NALU_IDR_SLICE) return RAP_NO;
			gf_bs_skip_bytes(mdia->nalu_parser, size - 1);
		}
	}
	return RAP_NO;
}