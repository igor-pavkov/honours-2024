static int snd_pcm_oss_unregister_minor(struct snd_pcm *pcm)
{
	snd_pcm_oss_disconnect_minor(pcm);
	snd_pcm_oss_proc_done(pcm);
	return 0;
}