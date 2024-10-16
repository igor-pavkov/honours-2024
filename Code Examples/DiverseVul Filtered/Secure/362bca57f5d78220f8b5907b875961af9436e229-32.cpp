static int snd_pcm_dev_register(struct snd_device *device)
{
	int cidx, err;
	struct snd_pcm_substream *substream;
	struct snd_pcm *pcm;

	if (snd_BUG_ON(!device || !device->device_data))
		return -ENXIO;
	pcm = device->device_data;

	mutex_lock(&register_mutex);
	err = snd_pcm_add(pcm);
	if (err)
		goto unlock;
	for (cidx = 0; cidx < 2; cidx++) {
		int devtype = -1;
		if (pcm->streams[cidx].substream == NULL)
			continue;
		switch (cidx) {
		case SNDRV_PCM_STREAM_PLAYBACK:
			devtype = SNDRV_DEVICE_TYPE_PCM_PLAYBACK;
			break;
		case SNDRV_PCM_STREAM_CAPTURE:
			devtype = SNDRV_DEVICE_TYPE_PCM_CAPTURE;
			break;
		}
		/* register pcm */
		err = snd_register_device(devtype, pcm->card, pcm->device,
					  &snd_pcm_f_ops[cidx], pcm,
					  &pcm->streams[cidx].dev);
		if (err < 0) {
			list_del_init(&pcm->list);
			goto unlock;
		}

		for (substream = pcm->streams[cidx].substream; substream; substream = substream->next)
			snd_pcm_timer_init(substream);
	}

	pcm_call_notify(pcm, n_register);

 unlock:
	mutex_unlock(&register_mutex);
	return err;
}