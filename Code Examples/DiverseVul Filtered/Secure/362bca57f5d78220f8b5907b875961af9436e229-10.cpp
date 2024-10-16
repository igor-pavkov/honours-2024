static void snd_pcm_substream_proc_info_read(struct snd_info_entry *entry,
					     struct snd_info_buffer *buffer)
{
	snd_pcm_proc_info_read(entry->private_data, buffer);
}