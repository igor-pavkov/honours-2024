snd_pcm_sframes_t snd_pcm_oss_read3(struct snd_pcm_substream *substream, char *ptr, snd_pcm_uframes_t frames, int in_kernel)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_sframes_t delay;
	int ret;
	while (1) {
		if (runtime->status->state == SNDRV_PCM_STATE_XRUN ||
		    runtime->status->state == SNDRV_PCM_STATE_SUSPENDED) {
#ifdef OSS_DEBUG
			pcm_dbg(substream->pcm,
				"pcm_oss: read: recovering from %s\n",
				runtime->status->state == SNDRV_PCM_STATE_XRUN ?
				"XRUN" : "SUSPEND");
#endif
			ret = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DRAIN, NULL);
			if (ret < 0)
				break;
		} else if (runtime->status->state == SNDRV_PCM_STATE_SETUP) {
			ret = snd_pcm_oss_prepare(substream);
			if (ret < 0)
				break;
		}
		ret = snd_pcm_oss_capture_position_fixup(substream, &delay);
		if (ret < 0)
			break;
		mutex_unlock(&runtime->oss.params_lock);
		ret = __snd_pcm_lib_xfer(substream, (void *)ptr, true,
					 frames, in_kernel);
		mutex_lock(&runtime->oss.params_lock);
		if (ret == -EPIPE) {
			if (runtime->status->state == SNDRV_PCM_STATE_DRAINING) {
				ret = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DROP, NULL);
				if (ret < 0)
					break;
			}
			continue;
		}
		if (ret != -ESTRPIPE)
			break;
	}
	return ret;
}