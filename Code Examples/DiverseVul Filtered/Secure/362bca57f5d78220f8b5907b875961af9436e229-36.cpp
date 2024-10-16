static const char *snd_pcm_access_name(snd_pcm_access_t access)
{
	return snd_pcm_access_names[(__force int)access];
}