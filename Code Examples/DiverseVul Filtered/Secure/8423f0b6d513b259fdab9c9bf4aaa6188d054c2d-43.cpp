static int snd_pcm_oss_set_channels(struct snd_pcm_oss_file *pcm_oss_file, unsigned int channels)
{
	int idx;
	if (channels < 1)
		channels = 1;
	if (channels > 128)
		return -EINVAL;
	for (idx = 1; idx >= 0; --idx) {
		struct snd_pcm_substream *substream = pcm_oss_file->streams[idx];
		struct snd_pcm_runtime *runtime;
		int err;

		if (substream == NULL)
			continue;
		runtime = substream->runtime;
		err = lock_params(runtime);
		if (err < 0)
			return err;
		if (runtime->oss.channels != channels) {
			runtime->oss.params = 1;
			runtime->oss.channels = channels;
		}
		unlock_params(runtime);
	}
	return snd_pcm_oss_get_channels(pcm_oss_file);
}