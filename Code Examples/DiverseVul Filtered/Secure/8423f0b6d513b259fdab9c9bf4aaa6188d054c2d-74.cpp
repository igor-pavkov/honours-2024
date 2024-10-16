static void snd_pcm_oss_proc_done(struct snd_pcm *pcm)
{
	int stream;
	for (stream = 0; stream < 2; ++stream) {
		struct snd_pcm_str *pstr = &pcm->streams[stream];
		snd_info_free_entry(pstr->oss.proc_entry);
		pstr->oss.proc_entry = NULL;
		snd_pcm_oss_proc_free_setup_list(pstr);
	}
}