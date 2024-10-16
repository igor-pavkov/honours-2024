static struct snd_pcm *snd_pcm_get(struct snd_card *card, int device)
{
	struct snd_pcm *pcm;

	list_for_each_entry(pcm, &snd_pcm_devices, list) {
		if (pcm->card == card && pcm->device == device)
			return pcm;
	}
	return NULL;
}