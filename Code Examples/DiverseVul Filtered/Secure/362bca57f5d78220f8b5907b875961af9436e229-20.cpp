static void snd_pcm_proc_init(void)
{
	struct snd_info_entry *entry;

	entry = snd_info_create_module_entry(THIS_MODULE, "pcm", NULL);
	if (entry) {
		snd_info_set_text_ops(entry, NULL, snd_pcm_proc_read);
		if (snd_info_register(entry) < 0) {
			snd_info_free_entry(entry);
			entry = NULL;
		}
	}
	snd_pcm_proc_entry = entry;
}