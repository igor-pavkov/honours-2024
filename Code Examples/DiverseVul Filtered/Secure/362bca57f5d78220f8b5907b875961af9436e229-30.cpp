static void snd_pcm_stream_proc_info_read(struct snd_info_entry *entry,
					  struct snd_info_buffer *buffer)
{
	snd_pcm_proc_info_read(((struct snd_pcm_str *)entry->private_data)->substream,
			       buffer);
}