static const char *snd_pcm_oss_format_name(int format)
{
	switch (format) {
	case AFMT_MU_LAW:
		return "MU_LAW";
	case AFMT_A_LAW:
		return "A_LAW";
	case AFMT_IMA_ADPCM:
		return "IMA_ADPCM";
	case AFMT_U8:
		return "U8";
	case AFMT_S16_LE:
		return "S16_LE";
	case AFMT_S16_BE:
		return "S16_BE";
	case AFMT_S8:
		return "S8";
	case AFMT_U16_LE:
		return "U16_LE";
	case AFMT_U16_BE:
		return "U16_BE";
	case AFMT_MPEG:
		return "MPEG";
	default:
		return "unknown";
	}
}