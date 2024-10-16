static void snd_pcm_xrun_debug_write(struct snd_info_entry *entry,
				     struct snd_info_buffer *buffer)
{
	struct snd_pcm_str *pstr = entry->private_data;
	char line[64];
	if (!snd_info_get_line(buffer, line, sizeof(line)))
		pstr->xrun_debug = simple_strtoul(line, NULL, 10);
}