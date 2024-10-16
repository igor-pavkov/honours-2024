static int __init alsa_pcm_init(void)
{
	snd_ctl_register_ioctl(snd_pcm_control_ioctl);
	snd_ctl_register_ioctl_compat(snd_pcm_control_ioctl);
	snd_pcm_proc_init();
	return 0;
}