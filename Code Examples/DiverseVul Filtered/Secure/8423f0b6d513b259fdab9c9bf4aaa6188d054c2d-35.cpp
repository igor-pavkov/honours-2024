static int snd_pcm_oss_set_format(struct snd_pcm_oss_file *pcm_oss_file, int format)
{
	int formats, idx;
	int err;
	
	if (format != AFMT_QUERY) {
		formats = snd_pcm_oss_get_formats(pcm_oss_file);
		if (formats < 0)
			return formats;
		if (!(formats & format))
			format = AFMT_U8;
		for (idx = 1; idx >= 0; --idx) {
			struct snd_pcm_substream *substream = pcm_oss_file->streams[idx];
			struct snd_pcm_runtime *runtime;
			if (substream == NULL)
				continue;
			runtime = substream->runtime;
			err = lock_params(runtime);
			if (err < 0)
				return err;
			if (runtime->oss.format != format) {
				runtime->oss.params = 1;
				runtime->oss.format = format;
			}
			unlock_params(runtime);
		}
	}
	return snd_pcm_oss_get_format(pcm_oss_file);
}