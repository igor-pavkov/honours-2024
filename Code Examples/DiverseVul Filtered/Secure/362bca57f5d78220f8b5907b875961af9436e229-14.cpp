static int snd_pcm_dev_free(struct snd_device *device)
{
	struct snd_pcm *pcm = device->device_data;
	return snd_pcm_free(pcm);
}