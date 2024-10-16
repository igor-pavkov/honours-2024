static void snd_pcm_substream_proc_hw_params_read(struct snd_info_entry *entry,
						  struct snd_info_buffer *buffer)
{
	struct snd_pcm_substream *substream = entry->private_data;
	struct snd_pcm_runtime *runtime;

	mutex_lock(&substream->pcm->open_mutex);
	runtime = substream->runtime;
	if (!runtime) {
		snd_iprintf(buffer, "closed\n");
		goto unlock;
	}
	if (runtime->status->state == SNDRV_PCM_STATE_OPEN) {
		snd_iprintf(buffer, "no setup\n");
		goto unlock;
	}
	snd_iprintf(buffer, "access: %s\n", snd_pcm_access_name(runtime->access));
	snd_iprintf(buffer, "format: %s\n", snd_pcm_format_name(runtime->format));
	snd_iprintf(buffer, "subformat: %s\n", snd_pcm_subformat_name(runtime->subformat));
	snd_iprintf(buffer, "channels: %u\n", runtime->channels);	
	snd_iprintf(buffer, "rate: %u (%u/%u)\n", runtime->rate, runtime->rate_num, runtime->rate_den);	
	snd_iprintf(buffer, "period_size: %lu\n", runtime->period_size);	
	snd_iprintf(buffer, "buffer_size: %lu\n", runtime->buffer_size);	
#if IS_ENABLED(CONFIG_SND_PCM_OSS)
	if (substream->oss.oss) {
		snd_iprintf(buffer, "OSS format: %s\n", snd_pcm_oss_format_name(runtime->oss.format));
		snd_iprintf(buffer, "OSS channels: %u\n", runtime->oss.channels);	
		snd_iprintf(buffer, "OSS rate: %u\n", runtime->oss.rate);
		snd_iprintf(buffer, "OSS period bytes: %lu\n", (unsigned long)runtime->oss.period_bytes);
		snd_iprintf(buffer, "OSS periods: %u\n", runtime->oss.periods);
		snd_iprintf(buffer, "OSS period frames: %lu\n", (unsigned long)runtime->oss.period_frames);
	}
#endif
 unlock:
	mutex_unlock(&substream->pcm->open_mutex);
}