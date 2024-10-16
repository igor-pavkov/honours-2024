GF_Err gf_isom_enum_sample_aux_data(GF_ISOFile *the_file, u32 trackNumber, u32 sample_number, u32 *sai_idx, u32 *sai_type, u32 *sai_parameter, u8 **sai_data, u32 *sai_size)
{
	GF_TrackBox *trak;
	u32 i, count;

	if (!sai_type || !sai_idx || !sai_data || !sai_size) return GF_BAD_PARAM;
	if (sai_parameter) *sai_parameter = 0;
	*sai_type = 0;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->sai_sizes) return GF_OK;
	if (!trak->Media->information->sampleTable->sai_offsets) return GF_OK;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sample_number <= trak->sample_count_at_seg_start) return GF_BAD_PARAM;
	sample_number -= trak->sample_count_at_seg_start;
#endif

	count = gf_list_count(trak->Media->information->sampleTable->sai_sizes);
	for (i=0; i<count; i++) {
		GF_Err e;
		GF_SampleAuxiliaryInfoSizeBox *saiz;
		GF_SampleAuxiliaryInfoOffsetBox *saio=NULL;
		u32 j;
		saiz = (GF_SampleAuxiliaryInfoSizeBox*)gf_list_get(trak->Media->information->sampleTable->sai_sizes, i);

		switch (saiz->aux_info_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
		case GF_ISOM_PIFF_SCHEME:
		case 0:
			continue;
		default:
			break;
		}
		if (*sai_idx>i) continue;

		for (j=0; j<gf_list_count(trak->Media->information->sampleTable->sai_offsets); j++) {
			saio = (GF_SampleAuxiliaryInfoOffsetBox*)gf_list_get(trak->Media->information->sampleTable->sai_offsets, j);
			if ((saio->aux_info_type == saiz->aux_info_type) && (saio->aux_info_type_parameter == saiz->aux_info_type_parameter)) break;
			saio = NULL;
		}
		if (!saio) continue;
		if (!saio->offsets && !saio->sai_data) continue;

		u64 offset = saio->offsets ? saio->offsets[0] : 0;
		u32 nb_saio = saio->entry_count;
		if ((nb_saio>1) && (saio->entry_count != saiz->sample_count)) continue;

		*sai_type = saiz->aux_info_type;
		if (sai_parameter) *sai_parameter = saiz->aux_info_type_parameter;

		(*sai_idx)++;

		if (nb_saio == 1) {
			for (j=0; j < sample_number-1; j++) {
				u32 size = saiz->default_sample_info_size ? saiz->default_sample_info_size : saiz->sample_info_size[j];
				offset += size;
			}
		} else {
			offset = saio->offsets[sample_number-1];
		}

		*sai_size = saiz->default_sample_info_size ? saiz->default_sample_info_size : saiz->sample_info_size[j];
		if (*sai_size) {
			*sai_data = gf_malloc( *sai_size);
			if (! *sai_data) return GF_OUT_OF_MEM;
		}

		e = GF_OK;
		if (saio->sai_data) {
			if (offset + *sai_size <= saio->sai_data->dataSize) {
				memcpy(*sai_data, saio->sai_data->data + offset, *sai_size);
			} else {
				e = GF_IO_ERR;
			}
		} else {
			u64 cur_position = gf_bs_get_position(the_file->movieFileMap->bs);
			gf_bs_seek(the_file->movieFileMap->bs, offset);

			u32 nb_read = gf_bs_read_data(the_file->movieFileMap->bs, *sai_data, *sai_size);
			if (nb_read != *sai_size) e = GF_IO_ERR;
			gf_bs_seek(the_file->movieFileMap->bs, cur_position);
		}

		if (e) {
			gf_free(*sai_data);
			*sai_data = NULL;
			*sai_size = 0;
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isobmf] Failed to clone sai data: %s\n", gf_error_to_string(e) ));
		}
		return e;
	}
	return GF_OK;
}