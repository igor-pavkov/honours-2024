static const char *snd_pcm_subformat_name(snd_pcm_subformat_t subformat)
{
	return snd_pcm_subformat_names[(__force int)subformat];
}