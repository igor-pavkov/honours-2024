static int snd_pcm_oss_release(struct inode *inode, struct file *file)
{
	struct snd_pcm *pcm;
	struct snd_pcm_substream *substream;
	struct snd_pcm_oss_file *pcm_oss_file;

	pcm_oss_file = file->private_data;
	substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
	if (substream == NULL)
		substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_CAPTURE];
	if (snd_BUG_ON(!substream))
		return -ENXIO;
	pcm = substream->pcm;
	if (!pcm->card->shutdown)
		snd_pcm_oss_sync(pcm_oss_file);
	mutex_lock(&pcm->open_mutex);
	snd_pcm_oss_release_file(pcm_oss_file);
	mutex_unlock(&pcm->open_mutex);
	wake_up(&pcm->open_wait);
	module_put(pcm->card->module);
	snd_card_file_remove(pcm->card, file);
	return 0;
}