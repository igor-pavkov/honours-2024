static GF_Err isom_cenc_get_sai_by_saiz_saio(GF_MediaBox *mdia, u32 sampleNumber, u32 scheme_type, u8 **out_buffer, u32 *out_size)
{
	u32  prev_sai_size, size, i, j, nb_saio;
	u64 cur_position, offset;
	GF_Err e = GF_OK;
	GF_SampleAuxiliaryInfoOffsetBox *saio_cenc=NULL;
	GF_SampleAuxiliaryInfoSizeBox *saiz_cenc=NULL;
	nb_saio = size = prev_sai_size = 0;
	offset = 0;

	if (! out_buffer || !out_size) return GF_BAD_PARAM;

	for (i = 0; i < gf_list_count(mdia->information->sampleTable->sai_offsets); i++) {
		GF_SampleAuxiliaryInfoOffsetBox *saio = (GF_SampleAuxiliaryInfoOffsetBox *)gf_list_get(mdia->information->sampleTable->sai_offsets, i);
		u32 aux_info_type = saio->aux_info_type;
		if (!aux_info_type) aux_info_type = scheme_type;

		switch (aux_info_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
		case GF_ISOM_PIFF_SCHEME:
			break;
		default:
			continue;
		}

		if (saio->entry_count == 1)
			offset = saio->offsets[0];
		else
			offset = saio->offsets[sampleNumber-1];
		nb_saio = saio->entry_count;
		saio_cenc = saio;
		break;
	}
	if (!saio_cenc) return GF_ISOM_INVALID_FILE;

	for (i = 0; i < gf_list_count(mdia->information->sampleTable->sai_sizes); i++) {
		GF_SampleAuxiliaryInfoSizeBox *saiz = (GF_SampleAuxiliaryInfoSizeBox *)gf_list_get(mdia->information->sampleTable->sai_sizes, i);
		u32 aux_info_type = saiz->aux_info_type;
		if (!aux_info_type) aux_info_type = scheme_type;

		switch (aux_info_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
		case GF_ISOM_PIFF_SCHEME:
			break;
		default:
			continue;
		}
		if (sampleNumber>saiz->sample_count) {
			return GF_NON_COMPLIANT_BITSTREAM;
		}
		if ((nb_saio==1) && !saio_cenc->total_size) {
			for (j = 0; j < saiz->sample_count; j++) {
				saio_cenc->total_size += saiz->default_sample_info_size ? saiz->default_sample_info_size : saiz->sample_info_size[j];
			}
		}
		if (saiz->cached_sample_num+1== sampleNumber) {
			prev_sai_size = saiz->cached_prev_size;
		} else {
			for (j = 0; j < sampleNumber-1; j++)
				prev_sai_size += saiz->default_sample_info_size ? saiz->default_sample_info_size : saiz->sample_info_size[j];
		}
		size = saiz->default_sample_info_size ? saiz->default_sample_info_size : saiz->sample_info_size[sampleNumber-1];
		saiz_cenc=saiz;
		break;
	}
	if (!saiz_cenc) return GF_BAD_PARAM;

	saiz_cenc->cached_sample_num = sampleNumber;
	saiz_cenc->cached_prev_size = prev_sai_size + size;

	if (saio_cenc->total_size) {
		if (!saio_cenc->cached_data) {
			saio_cenc->cached_data = gf_malloc(sizeof(u8)*saio_cenc->total_size);
			if (!saio_cenc->cached_data) return GF_OUT_OF_MEM;
			cur_position = gf_bs_get_position(mdia->information->dataHandler->bs);
			gf_bs_seek(mdia->information->dataHandler->bs, offset);
			gf_bs_read_data(mdia->information->dataHandler->bs, saio_cenc->cached_data, saio_cenc->total_size);
			gf_bs_seek(mdia->information->dataHandler->bs, cur_position);
		}
		if (out_size) {
			if (out_buffer) {
				if ((*out_size) < size) {
					(*out_buffer) = gf_realloc((*out_buffer), sizeof(char)*(size) );
					if (! *out_buffer) return GF_OUT_OF_MEM;
				}
				memcpy((*out_buffer), saio_cenc->cached_data + prev_sai_size, size);
			}
			(*out_size) = size;
		}
		return GF_OK;
	}

	offset += (nb_saio == 1) ? prev_sai_size : 0;
	cur_position = gf_bs_get_position(mdia->information->dataHandler->bs);
	gf_bs_seek(mdia->information->dataHandler->bs, offset);

	if (out_buffer) {
		if ((*out_size) < size) {
			(*out_buffer) = gf_realloc((*out_buffer), sizeof(char)*(size) );
			if (! *out_buffer) return GF_OUT_OF_MEM;
		}
		gf_bs_read_data(mdia->information->dataHandler->bs, (*out_buffer), size);
	}
	(*out_size) = size;

	gf_bs_seek(mdia->information->dataHandler->bs, cur_position);

	return e;
}