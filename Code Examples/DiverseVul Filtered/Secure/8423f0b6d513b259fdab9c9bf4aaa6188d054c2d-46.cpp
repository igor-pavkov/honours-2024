static __poll_t snd_pcm_oss_poll(struct file *file, poll_table * wait)
{
	struct snd_pcm_oss_file *pcm_oss_file;
	__poll_t mask;
	struct snd_pcm_substream *psubstream = NULL, *csubstream = NULL;
	
	pcm_oss_file = file->private_data;

	psubstream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
	csubstream = pcm_oss_file->streams[SNDRV_PCM_STREAM_CAPTURE];

	mask = 0;
	if (psubstream != NULL) {
		struct snd_pcm_runtime *runtime = psubstream->runtime;
		poll_wait(file, &runtime->sleep, wait);
		snd_pcm_stream_lock_irq(psubstream);
		if (runtime->status->state != SNDRV_PCM_STATE_DRAINING &&
		    (runtime->status->state != SNDRV_PCM_STATE_RUNNING ||
		     snd_pcm_oss_playback_ready(psubstream)))
			mask |= EPOLLOUT | EPOLLWRNORM;
		snd_pcm_stream_unlock_irq(psubstream);
	}
	if (csubstream != NULL) {
		struct snd_pcm_runtime *runtime = csubstream->runtime;
		snd_pcm_state_t ostate;
		poll_wait(file, &runtime->sleep, wait);
		snd_pcm_stream_lock_irq(csubstream);
		ostate = runtime->status->state;
		if (ostate != SNDRV_PCM_STATE_RUNNING ||
		    snd_pcm_oss_capture_ready(csubstream))
			mask |= EPOLLIN | EPOLLRDNORM;
		snd_pcm_stream_unlock_irq(csubstream);
		if (ostate != SNDRV_PCM_STATE_RUNNING && runtime->oss.trigger) {
			struct snd_pcm_oss_file ofile;
			memset(&ofile, 0, sizeof(ofile));
			ofile.streams[SNDRV_PCM_STREAM_CAPTURE] = pcm_oss_file->streams[SNDRV_PCM_STREAM_CAPTURE];
			runtime->oss.trigger = 0;
			snd_pcm_oss_set_trigger(&ofile, PCM_ENABLE_INPUT);
		}
	}

	return mask;
}