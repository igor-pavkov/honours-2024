GF_Err avcc_box_size(GF_Box *s)
{
	u32 i, count;
	GF_AVCConfigurationBox *ptr = (GF_AVCConfigurationBox *)s;

	if (!ptr->config) {
		ptr->size = 0;
		return GF_OK;
	}
	ptr->size += 7;
	count = gf_list_count(ptr->config->sequenceParameterSets);
	for (i=0; i<count; i++)
		ptr->size += 2 + ((GF_NALUFFParam *)gf_list_get(ptr->config->sequenceParameterSets, i))->size;

	count = gf_list_count(ptr->config->pictureParameterSets);
	for (i=0; i<count; i++)
		ptr->size += 2 + ((GF_NALUFFParam *)gf_list_get(ptr->config->pictureParameterSets, i))->size;

	if (ptr->type==GF_ISOM_BOX_TYPE_AVCC) {
		if (gf_avc_is_rext_profile(ptr->config->AVCProfileIndication)) {
			ptr->size += 4;
			count = ptr->config->sequenceParameterSetExtensions ?gf_list_count(ptr->config->sequenceParameterSetExtensions) : 0;
			for (i=0; i<count; i++)
				ptr->size += 2 + ((GF_NALUFFParam *)gf_list_get(ptr->config->sequenceParameterSetExtensions, i))->size;
		}
	}
	return GF_OK;
}