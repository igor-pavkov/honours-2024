static int snd_pcm_oss_set_rate(struct snd_pcm_oss_file *pcm_oss_file, int rate)
{
	int idx;

	for (idx = 1; idx >= 0; --idx) {
		struct snd_pcm_substream *substream = pcm_oss_file->streams[idx];
		struct snd_pcm_runtime *runtime;
		int err;

		if (substream == NULL)
			continue;
		runtime = substream->runtime;
		if (rate < 1000)
			rate = 1000;
		else if (rate > 192000)
			rate = 192000;
		err = lock_params(runtime);
		if (err < 0)
			return err;
		if (runtime->oss.rate != rate) {
			runtime->oss.params = 1;
			runtime->oss.rate = rate;
		}
		unlock_params(runtime);
	}
	return snd_pcm_oss_get_rate(pcm_oss_file);
}