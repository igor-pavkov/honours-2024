static void snd_pcm_xrun_debug_read(struct snd_info_entry *entry,
				    struct snd_info_buffer *buffer)
{
	struct snd_pcm_str *pstr = entry->private_data;
	snd_iprintf(buffer, "%d\n", pstr->xrun_debug);
}