static int snd_pcm_oss_make_ready_locked(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime;
	int err;

	runtime = substream->runtime;
	if (runtime->oss.params) {
		err = snd_pcm_oss_change_params_locked(substream);
		if (err < 0)
			return err;
	}
	if (runtime->oss.prepare) {
		err = snd_pcm_oss_prepare(substream);
		if (err < 0)
			return err;
	}
	return 0;
}