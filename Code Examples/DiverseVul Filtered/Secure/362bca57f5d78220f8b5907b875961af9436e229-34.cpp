static void snd_pcm_proc_done(void)
{
	snd_info_free_entry(snd_pcm_proc_entry);
}