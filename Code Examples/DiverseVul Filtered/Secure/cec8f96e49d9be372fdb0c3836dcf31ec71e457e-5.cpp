static int snd_timer_user_gparams(struct file *file,
				  struct snd_timer_gparams __user *_gparams)
{
	struct snd_timer_gparams gparams;

	if (copy_from_user(&gparams, _gparams, sizeof(gparams)))
		return -EFAULT;
	return timer_set_gparams(&gparams);
}