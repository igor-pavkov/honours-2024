static void snd_pcm_oss_proc_init(struct snd_pcm *pcm)
{
	int stream;
	for (stream = 0; stream < 2; ++stream) {
		struct snd_info_entry *entry;
		struct snd_pcm_str *pstr = &pcm->streams[stream];
		if (pstr->substream_count == 0)
			continue;
		entry = snd_info_create_card_entry(pcm->card, "oss", pstr->proc_root);
		if (entry) {
			entry->content = SNDRV_INFO_CONTENT_TEXT;
			entry->mode = S_IFREG | 0644;
			entry->c.text.read = snd_pcm_oss_proc_read;
			entry->c.text.write = snd_pcm_oss_proc_write;
			entry->private_data = pstr;
			if (snd_info_register(entry) < 0) {
				snd_info_free_entry(entry);
				entry = NULL;
			}
		}
		pstr->oss.proc_entry = entry;
	}
}