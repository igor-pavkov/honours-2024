snd_pcm_sframes_t snd_pcm_oss_writev3(struct snd_pcm_substream *substream, void **bufs, snd_pcm_uframes_t frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret;
	while (1) {
		if (runtime->status->state == SNDRV_PCM_STATE_XRUN ||
		    runtime->status->state == SNDRV_PCM_STATE_SUSPENDED) {
#ifdef OSS_DEBUG
			pcm_dbg(substream->pcm,
				"pcm_oss: writev: recovering from %s\n",
				runtime->status->state == SNDRV_PCM_STATE_XRUN ?
				"XRUN" : "SUSPEND");
#endif
			ret = snd_pcm_oss_prepare(substream);
			if (ret < 0)
				break;
		}
		ret = snd_pcm_kernel_writev(substream, bufs, frames);
		if (ret != -EPIPE && ret != -ESTRPIPE)
			break;

		/* test, if we can't store new data, because the stream */
		/* has not been started */
		if (runtime->status->state == SNDRV_PCM_STATE_PREPARED)
			return -EAGAIN;
	}
	return ret;
}