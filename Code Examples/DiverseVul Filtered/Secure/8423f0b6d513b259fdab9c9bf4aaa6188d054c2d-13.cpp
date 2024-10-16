static int snd_pcm_oss_capture_position_fixup(struct snd_pcm_substream *substream, snd_pcm_sframes_t *delay)
{
	struct snd_pcm_runtime *runtime;
	snd_pcm_uframes_t frames;
	int err = 0;

	while (1) {
		err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DELAY, delay);
		if (err < 0)
			break;
		runtime = substream->runtime;
		if (*delay <= (snd_pcm_sframes_t)runtime->buffer_size)
			break;
		/* in case of overrun, skip whole periods like OSS/Linux driver does */
		/* until avail(delay) <= buffer_size */
		frames = (*delay - runtime->buffer_size) + runtime->period_size - 1;
		frames /= runtime->period_size;
		frames *= runtime->period_size;
		err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_FORWARD, &frames);
		if (err < 0)
			break;
	}
	return err;
}