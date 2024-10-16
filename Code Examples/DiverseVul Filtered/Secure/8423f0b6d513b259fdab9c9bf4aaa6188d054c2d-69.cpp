static long snd_pcm_oss_ioctl_compat(struct file *file, unsigned int cmd,
				     unsigned long arg)
{
	/*
	 * Everything is compatbile except SNDCTL_DSP_MAPINBUF/SNDCTL_DSP_MAPOUTBUF,
	 * which are not implemented for the native case either
	 */
	return snd_pcm_oss_ioctl(file, cmd, (unsigned long)compat_ptr(arg));
}