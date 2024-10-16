static int snd_pcm_oss_sync1(struct snd_pcm_substream *substream, size_t size)
{
	struct snd_pcm_runtime *runtime;
	ssize_t result = 0;
	snd_pcm_state_t state;
	long res;
	wait_queue_entry_t wait;

	runtime = substream->runtime;
	init_waitqueue_entry(&wait, current);
	add_wait_queue(&runtime->sleep, &wait);
#ifdef OSS_DEBUG
	pcm_dbg(substream->pcm, "sync1: size = %li\n", size);
#endif
	while (1) {
		result = snd_pcm_oss_write2(substream, runtime->oss.buffer, size, 1);
		if (result > 0) {
			runtime->oss.buffer_used = 0;
			result = 0;
			break;
		}
		if (result != 0 && result != -EAGAIN)
			break;
		result = 0;
		set_current_state(TASK_INTERRUPTIBLE);
		snd_pcm_stream_lock_irq(substream);
		state = runtime->status->state;
		snd_pcm_stream_unlock_irq(substream);
		if (state != SNDRV_PCM_STATE_RUNNING) {
			set_current_state(TASK_RUNNING);
			break;
		}
		res = schedule_timeout(10 * HZ);
		if (signal_pending(current)) {
			result = -ERESTARTSYS;
			break;
		}
		if (res == 0) {
			pcm_err(substream->pcm,
				"OSS sync error - DMA timeout\n");
			result = -EIO;
			break;
		}
	}
	remove_wait_queue(&runtime->sleep, &wait);
	return result;
}