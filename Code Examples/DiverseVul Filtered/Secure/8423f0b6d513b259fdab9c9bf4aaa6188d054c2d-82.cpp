static ssize_t snd_pcm_oss_read1(struct snd_pcm_substream *substream, char __user *buf, size_t bytes)
{
	size_t xfer = 0;
	ssize_t tmp = 0;
	struct snd_pcm_runtime *runtime = substream->runtime;

	if (atomic_read(&substream->mmap_count))
		return -ENXIO;

	atomic_inc(&runtime->oss.rw_ref);
	while (bytes > 0) {
		if (mutex_lock_interruptible(&runtime->oss.params_lock)) {
			tmp = -ERESTARTSYS;
			break;
		}
		tmp = snd_pcm_oss_make_ready_locked(substream);
		if (tmp < 0)
			goto err;
		if (bytes < runtime->oss.period_bytes || runtime->oss.buffer_used > 0) {
			if (runtime->oss.buffer_used == 0) {
				tmp = snd_pcm_oss_read2(substream, runtime->oss.buffer, runtime->oss.period_bytes, 1);
				if (tmp <= 0)
					goto err;
				runtime->oss.bytes += tmp;
				runtime->oss.period_ptr = tmp;
				runtime->oss.buffer_used = tmp;
			}
			tmp = bytes;
			if ((size_t) tmp > runtime->oss.buffer_used)
				tmp = runtime->oss.buffer_used;
			if (copy_to_user(buf, runtime->oss.buffer + (runtime->oss.period_ptr - runtime->oss.buffer_used), tmp)) {
				tmp = -EFAULT;
				goto err;
			}
			buf += tmp;
			bytes -= tmp;
			xfer += tmp;
			runtime->oss.buffer_used -= tmp;
		} else {
			tmp = snd_pcm_oss_read2(substream, (char __force *)buf,
						runtime->oss.period_bytes, 0);
			if (tmp <= 0)
				goto err;
			runtime->oss.bytes += tmp;
			buf += tmp;
			bytes -= tmp;
			xfer += tmp;
		}
 err:
		mutex_unlock(&runtime->oss.params_lock);
		if (tmp < 0)
			break;
		if (signal_pending(current)) {
			tmp = -ERESTARTSYS;
			break;
		}
		tmp = 0;
	}
	atomic_dec(&runtime->oss.rw_ref);
	return xfer > 0 ? (snd_pcm_sframes_t)xfer : tmp;
}