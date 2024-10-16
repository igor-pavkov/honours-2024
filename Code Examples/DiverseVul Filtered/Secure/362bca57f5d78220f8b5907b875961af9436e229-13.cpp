static int snd_pcm_next(struct snd_card *card, int device)
{
	struct snd_pcm *pcm;

	list_for_each_entry(pcm, &snd_pcm_devices, list) {
		if (pcm->card == card && pcm->device > device)
			return pcm->device;
		else if (pcm->card->number > card->number)
			return -1;
	}
	return -1;
}