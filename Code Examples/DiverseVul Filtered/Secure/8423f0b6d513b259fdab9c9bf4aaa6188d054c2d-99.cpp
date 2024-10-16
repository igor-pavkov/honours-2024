static int snd_pcm_oss_prepare(struct snd_pcm_substream *substream)
{
	int err;
	struct snd_pcm_runtime *runtime = substream->runtime;

	err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_PREPARE, NULL);
	if (err < 0) {
		pcm_dbg(substream->pcm,
			"snd_pcm_oss_prepare: SNDRV_PCM_IOCTL_PREPARE failed\n");
		return err;
	}
	runtime->oss.prepare = 0;
	runtime->oss.prev_hw_ptr_period = 0;
	runtime->oss.period_ptr = 0;
	runtime->oss.buffer_used = 0;

	return 0;
}