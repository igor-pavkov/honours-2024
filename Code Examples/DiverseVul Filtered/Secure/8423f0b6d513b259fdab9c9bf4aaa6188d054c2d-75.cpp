static int snd_pcm_oss_reset(struct snd_pcm_oss_file *pcm_oss_file)
{
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;
	int i;

	for (i = 0; i < 2; i++) { 
		substream = pcm_oss_file->streams[i];
		if (!substream)
			continue;
		runtime = substream->runtime;
		snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DROP, NULL);
		mutex_lock(&runtime->oss.params_lock);
		runtime->oss.prepare = 1;
		runtime->oss.buffer_used = 0;
		runtime->oss.prev_hw_ptr_period = 0;
		runtime->oss.period_ptr = 0;
		mutex_unlock(&runtime->oss.params_lock);
	}
	return 0;
}