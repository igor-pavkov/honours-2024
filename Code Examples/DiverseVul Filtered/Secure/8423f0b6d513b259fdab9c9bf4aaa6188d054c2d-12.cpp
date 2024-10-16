static int snd_pcm_oss_release_file(struct snd_pcm_oss_file *pcm_oss_file)
{
	int cidx;
	if (!pcm_oss_file)
		return 0;
	for (cidx = 0; cidx < 2; ++cidx) {
		struct snd_pcm_substream *substream = pcm_oss_file->streams[cidx];
		if (substream)
			snd_pcm_release_substream(substream);
	}
	kfree(pcm_oss_file);
	return 0;
}