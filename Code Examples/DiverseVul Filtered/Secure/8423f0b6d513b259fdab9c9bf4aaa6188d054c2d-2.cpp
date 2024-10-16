static int snd_pcm_oss_format_to(snd_pcm_format_t format)
{
	switch (format) {
	case SNDRV_PCM_FORMAT_MU_LAW:	return AFMT_MU_LAW;
	case SNDRV_PCM_FORMAT_A_LAW:	return AFMT_A_LAW;
	case SNDRV_PCM_FORMAT_IMA_ADPCM:	return AFMT_IMA_ADPCM;
	case SNDRV_PCM_FORMAT_U8:		return AFMT_U8;
	case SNDRV_PCM_FORMAT_S16_LE:	return AFMT_S16_LE;
	case SNDRV_PCM_FORMAT_S16_BE:	return AFMT_S16_BE;
	case SNDRV_PCM_FORMAT_S8:		return AFMT_S8;
	case SNDRV_PCM_FORMAT_U16_LE:	return AFMT_U16_LE;
	case SNDRV_PCM_FORMAT_U16_BE:	return AFMT_U16_BE;
	case SNDRV_PCM_FORMAT_MPEG:		return AFMT_MPEG;
	case SNDRV_PCM_FORMAT_S32_LE:	return AFMT_S32_LE;
	case SNDRV_PCM_FORMAT_S32_BE:	return AFMT_S32_BE;
	case SNDRV_PCM_FORMAT_S24_LE:	return AFMT_S24_LE;
	case SNDRV_PCM_FORMAT_S24_BE:	return AFMT_S24_BE;
	case SNDRV_PCM_FORMAT_S24_3LE:	return AFMT_S24_PACKED;
	case SNDRV_PCM_FORMAT_FLOAT:	return AFMT_FLOAT;
	case SNDRV_PCM_FORMAT_IEC958_SUBFRAME: return AFMT_SPDIF_RAW;
	default:			return -EINVAL;
	}
}