static int snd_pcm_oss_get_odelay(struct snd_pcm_oss_file *pcm_oss_file)
{
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;
	snd_pcm_sframes_t delay;
	int err;

	substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
	if (substream == NULL)
		return -EINVAL;
	err = snd_pcm_oss_make_ready(substream);
	if (err < 0)
		return err;
	runtime = substream->runtime;
	if (runtime->oss.params || runtime->oss.prepare)
		return 0;
	err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DELAY, &delay);
	if (err == -EPIPE)
		delay = 0;	/* hack for broken OSS applications */
	else if (err < 0)
		return err;
	return snd_pcm_oss_bytes(substream, delay);
}