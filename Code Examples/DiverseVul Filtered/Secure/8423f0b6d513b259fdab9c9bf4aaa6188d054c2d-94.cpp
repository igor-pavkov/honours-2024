static void __exit alsa_pcm_oss_exit(void)
{
	snd_pcm_notify(&snd_pcm_oss_notify, 1);
}