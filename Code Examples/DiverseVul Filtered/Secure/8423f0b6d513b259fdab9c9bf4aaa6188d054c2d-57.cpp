static int snd_pcm_oss_change_params(struct snd_pcm_substream *substream,
				     bool trylock)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	if (trylock) {
		if (!(mutex_trylock(&runtime->oss.params_lock)))
			return -EAGAIN;
	} else if (mutex_lock_interruptible(&runtime->oss.params_lock))
		return -ERESTARTSYS;

	err = snd_pcm_oss_change_params_locked(substream);
	mutex_unlock(&runtime->oss.params_lock);
	return err;
}