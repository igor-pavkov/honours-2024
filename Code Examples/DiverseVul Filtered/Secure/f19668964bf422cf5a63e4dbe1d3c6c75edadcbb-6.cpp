GF_Err piff_psec_box_size(GF_Box *s)
{
	u32 i, sample_count;
	GF_SampleEncryptionBox *ptr = (GF_SampleEncryptionBox*)s;

	sample_count = gf_list_count(ptr->samp_aux_info);
	if (!sample_count) {
		ptr->size = 0;
		return GF_OK;
	}

	ptr->size += 4;
	if (ptr->flags & 1) {
		ptr->size += 20;
	}
	ptr->size += 4;

	for (i = 0; i < sample_count; i++) {
		GF_CENCSampleAuxInfo *sai = (GF_CENCSampleAuxInfo *)gf_list_get(ptr->samp_aux_info, i);
		if (! sai->cenc_data_size) continue;
		ptr->size += sai->cenc_data_size;
	}
	return GF_OK;
}