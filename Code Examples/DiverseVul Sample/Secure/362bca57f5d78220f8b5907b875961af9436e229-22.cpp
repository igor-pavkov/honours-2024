static void snd_pcm_proc_read(struct snd_info_entry *entry,
			      struct snd_info_buffer *buffer)
{
	struct snd_pcm *pcm;

	mutex_lock(&register_mutex);
	list_for_each_entry(pcm, &snd_pcm_devices, list) {
		snd_iprintf(buffer, "%02i-%02i: %s : %s",
			    pcm->card->number, pcm->device, pcm->id, pcm->name);
		if (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream)
			snd_iprintf(buffer, " : playback %i",
				    pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream_count);
		if (pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream)
			snd_iprintf(buffer, " : capture %i",
				    pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream_count);
		snd_iprintf(buffer, "\n");
	}
	mutex_unlock(&register_mutex);
}