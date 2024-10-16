GF_Err vpcc_box_size(GF_Box *s)
{
	GF_VPConfigurationBox *ptr = (GF_VPConfigurationBox *)s;

	if (!ptr->config) {
		ptr->size = 0;
		return GF_OK;
	}

	if (ptr->version == 0) {
		ptr->size += 6;
	} else {
		if (ptr->config->codec_initdata_size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[ISOBMFF] VPConfigurationBox: codec_initdata_size MUST be 0, was %d\n", ptr->config->codec_initdata_size));
			return GF_ISOM_INVALID_FILE;
		}

		ptr->size += 8;
	}

	return GF_OK;
}