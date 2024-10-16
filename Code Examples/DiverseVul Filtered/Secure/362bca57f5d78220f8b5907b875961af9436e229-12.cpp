static int snd_pcm_add(struct snd_pcm *newpcm)
{
	struct snd_pcm *pcm;

	if (newpcm->internal)
		return 0;

	list_for_each_entry(pcm, &snd_pcm_devices, list) {
		if (pcm->card == newpcm->card && pcm->device == newpcm->device)
			return -EBUSY;
		if (pcm->card->number > newpcm->card->number ||
				(pcm->card == newpcm->card &&
				pcm->device > newpcm->device)) {
			list_add(&newpcm->list, pcm->list.prev);
			return 0;
		}
	}
	list_add_tail(&newpcm->list, &snd_pcm_devices);
	return 0;
}