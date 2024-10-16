static int snd_pcm_oss_make_ready(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime;
	int err;

	runtime = substream->runtime;
	if (runtime->oss.params) {
		err = snd_pcm_oss_change_params(substream, false);
		if (err < 0)
			return err;
	}
	if (runtime->oss.prepare) {
		if (mutex_lock_interruptible(&runtime->oss.params_lock))
			return -ERESTARTSYS;
		err = snd_pcm_oss_prepare(substream);
		mutex_unlock(&runtime->oss.params_lock);
		if (err < 0)
			return err;
	}
	return 0;
}