static int lock_params(struct snd_pcm_runtime *runtime)
{
	if (mutex_lock_interruptible(&runtime->oss.params_lock))
		return -ERESTARTSYS;
	if (atomic_read(&runtime->oss.rw_ref)) {
		mutex_unlock(&runtime->oss.params_lock);
		return -EBUSY;
	}
	return 0;
}