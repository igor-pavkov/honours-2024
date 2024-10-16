static int snd_pcm_oss_set_trigger(struct snd_pcm_oss_file *pcm_oss_file, int trigger)
{
	struct snd_pcm_runtime *runtime;
	struct snd_pcm_substream *psubstream = NULL, *csubstream = NULL;
	int err, cmd;

#ifdef OSS_DEBUG
	pr_debug("pcm_oss: trigger = 0x%x\n", trigger);
#endif
	
	psubstream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
	csubstream = pcm_oss_file->streams[SNDRV_PCM_STREAM_CAPTURE];

	if (psubstream) {
		err = snd_pcm_oss_make_ready(psubstream);
		if (err < 0)
			return err;
	}
	if (csubstream) {
		err = snd_pcm_oss_make_ready(csubstream);
		if (err < 0)
			return err;
	}
      	if (psubstream) {
      		runtime = psubstream->runtime;
		cmd = 0;
		if (mutex_lock_interruptible(&runtime->oss.params_lock))
			return -ERESTARTSYS;
		if (trigger & PCM_ENABLE_OUTPUT) {
			if (runtime->oss.trigger)
				goto _skip1;
			if (atomic_read(&psubstream->mmap_count))
				snd_pcm_oss_simulate_fill(psubstream,
						get_hw_ptr_period(runtime));
			runtime->oss.trigger = 1;
			runtime->start_threshold = 1;
			cmd = SNDRV_PCM_IOCTL_START;
		} else {
			if (!runtime->oss.trigger)
				goto _skip1;
			runtime->oss.trigger = 0;
			runtime->start_threshold = runtime->boundary;
			cmd = SNDRV_PCM_IOCTL_DROP;
			runtime->oss.prepare = 1;
		}
 _skip1:
		mutex_unlock(&runtime->oss.params_lock);
		if (cmd) {
			err = snd_pcm_kernel_ioctl(psubstream, cmd, NULL);
			if (err < 0)
				return err;
		}
	}
	if (csubstream) {
      		runtime = csubstream->runtime;
		cmd = 0;
		if (mutex_lock_interruptible(&runtime->oss.params_lock))
			return -ERESTARTSYS;
		if (trigger & PCM_ENABLE_INPUT) {
			if (runtime->oss.trigger)
				goto _skip2;
			runtime->oss.trigger = 1;
			runtime->start_threshold = 1;
			cmd = SNDRV_PCM_IOCTL_START;
		} else {
			if (!runtime->oss.trigger)
				goto _skip2;
			runtime->oss.trigger = 0;
			runtime->start_threshold = runtime->boundary;
			cmd = SNDRV_PCM_IOCTL_DROP;
			runtime->oss.prepare = 1;
		}
 _skip2:
		mutex_unlock(&runtime->oss.params_lock);
		if (cmd) {
			err = snd_pcm_kernel_ioctl(csubstream, cmd, NULL);
			if (err < 0)
				return err;
		}
	}
	return 0;
}