static int snd_pcm_oss_get_block_size(struct snd_pcm_oss_file *pcm_oss_file)
{
	struct snd_pcm_substream *substream;
	int err;
	
	err = snd_pcm_oss_get_active_substream(pcm_oss_file, &substream);
	if (err < 0)
		return err;
	return substream->runtime->oss.period_bytes;
}