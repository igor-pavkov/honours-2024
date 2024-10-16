static int snd_pcm_oss_get_active_substream(struct snd_pcm_oss_file *pcm_oss_file, struct snd_pcm_substream **r_substream)
{
	int idx, err;
	struct snd_pcm_substream *asubstream = NULL, *substream;

	for (idx = 0; idx < 2; idx++) {
		substream = pcm_oss_file->streams[idx];
		if (substream == NULL)
			continue;
		if (asubstream == NULL)
			asubstream = substream;
		if (substream->runtime->oss.params) {
			err = snd_pcm_oss_change_params(substream, false);
			if (err < 0)
				return err;
		}
	}
	if (!asubstream)
		return -EIO;
	if (r_substream)
		*r_substream = asubstream;
	return 0;
}