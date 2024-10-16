int snd_pcm_new_internal(struct snd_card *card, const char *id, int device,
	int playback_count, int capture_count,
	struct snd_pcm **rpcm)
{
	return _snd_pcm_new(card, id, device, playback_count, capture_count,
			true, rpcm);
}