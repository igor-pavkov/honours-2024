static const char *snd_pcm_state_name(snd_pcm_state_t state)
{
	return snd_pcm_state_names[(__force int)state];
}