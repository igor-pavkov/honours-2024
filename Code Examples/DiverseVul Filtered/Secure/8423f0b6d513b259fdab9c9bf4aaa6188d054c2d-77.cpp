static int snd_pcm_oss_set_fragment(struct snd_pcm_oss_file *pcm_oss_file, unsigned int val)
{
	int err = -EINVAL, idx;

	for (idx = 1; idx >= 0; --idx) {
		struct snd_pcm_substream *substream = pcm_oss_file->streams[idx];
		struct snd_pcm_runtime *runtime;

		if (substream == NULL)
			continue;
		runtime = substream->runtime;
		err = lock_params(runtime);
		if (err < 0)
			return err;
		err = snd_pcm_oss_set_fragment1(substream, val);
		unlock_params(runtime);
		if (err < 0)
			return err;
	}
	return err;
}