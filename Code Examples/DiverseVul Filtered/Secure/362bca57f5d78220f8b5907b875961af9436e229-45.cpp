const char *snd_pcm_format_name(snd_pcm_format_t format)
{
	if ((__force unsigned int)format >= ARRAY_SIZE(snd_pcm_format_names))
		return "Unknown";
	return snd_pcm_format_names[(__force unsigned int)format];
}