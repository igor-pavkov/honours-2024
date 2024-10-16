snd_pcm_sframes_t snd_pcm_oss_readv3(struct snd_pcm_substream *substream, void **bufs, snd_pcm_uframes_t frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret;
	while (1) {
		if (runtime->status->state == SNDRV_PCM_STATE_XRUN ||
		    runtime->status->state == SNDRV_PCM_STATE_SUSPENDED) {
#ifdef OSS_DEBUG
			pcm_dbg(substream->pcm,
				"pcm_oss: readv: recovering from %s\n",
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
		ret = snd_pcm_kernel_readv(substream, bufs, frames);
		if (ret != -EPIPE && ret != -ESTRPIPE)
			break;
	}
	return ret;
}