static ssize_t snd_pcm_oss_write1(struct snd_pcm_substream *substream, const char __user *buf, size_t bytes)
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
			tmp = bytes;
			if (tmp + runtime->oss.buffer_used > runtime->oss.period_bytes)
				tmp = runtime->oss.period_bytes - runtime->oss.buffer_used;
			if (tmp > 0) {
				if (copy_from_user(runtime->oss.buffer + runtime->oss.buffer_used, buf, tmp)) {
					tmp = -EFAULT;
					goto err;
				}
			}
			runtime->oss.buffer_used += tmp;
			buf += tmp;
			bytes -= tmp;
			xfer += tmp;
			if (substream->oss.setup.partialfrag ||
			    runtime->oss.buffer_used == runtime->oss.period_bytes) {
				tmp = snd_pcm_oss_write2(substream, runtime->oss.buffer + runtime->oss.period_ptr, 
							 runtime->oss.buffer_used - runtime->oss.period_ptr, 1);
				if (tmp <= 0)
					goto err;
				runtime->oss.bytes += tmp;
				runtime->oss.period_ptr += tmp;
				runtime->oss.period_ptr %= runtime->oss.period_bytes;
				if (runtime->oss.period_ptr == 0 ||
				    runtime->oss.period_ptr == runtime->oss.buffer_used)
					runtime->oss.buffer_used = 0;
				else if ((substream->f_flags & O_NONBLOCK) != 0) {
					tmp = -EAGAIN;
					goto err;
				}
			}
		} else {
			tmp = snd_pcm_oss_write2(substream,
						 (const char __force *)buf,
						 runtime->oss.period_bytes, 0);
			if (tmp <= 0)
				goto err;
			runtime->oss.bytes += tmp;
			buf += tmp;
			bytes -= tmp;
			xfer += tmp;
			if ((substream->f_flags & O_NONBLOCK) != 0 &&
			    tmp != runtime->oss.period_bytes)
				tmp = -EAGAIN;
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