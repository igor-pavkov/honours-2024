static int snd_pcm_oss_get_trigger(struct snd_pcm_oss_file *pcm_oss_file)
{
	struct snd_pcm_substream *psubstream = NULL, *csubstream = NULL;
	int result = 0;

	psubstream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
	csubstream = pcm_oss_file->streams[SNDRV_PCM_STREAM_CAPTURE];
	if (psubstream && psubstream->runtime && psubstream->runtime->oss.trigger)
		result |= PCM_ENABLE_OUTPUT;
	if (csubstream && csubstream->runtime && csubstream->runtime->oss.trigger)
		result |= PCM_ENABLE_INPUT;
	return result;
}