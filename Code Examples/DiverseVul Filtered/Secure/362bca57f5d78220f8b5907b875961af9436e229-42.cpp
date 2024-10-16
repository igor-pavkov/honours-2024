static void __exit alsa_pcm_exit(void)
{
	snd_ctl_unregister_ioctl(snd_pcm_control_ioctl);
	snd_ctl_unregister_ioctl_compat(snd_pcm_control_ioctl);
	snd_pcm_proc_done();
}