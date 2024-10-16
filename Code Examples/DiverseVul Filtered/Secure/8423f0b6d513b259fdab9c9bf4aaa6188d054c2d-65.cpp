static int snd_pcm_oss_get_ptr(struct snd_pcm_oss_file *pcm_oss_file, int stream, struct count_info __user * _info)
{	
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;
	snd_pcm_sframes_t delay;
	int fixup;
	struct count_info info;
	int err;

	if (_info == NULL)
		return -EFAULT;
	substream = pcm_oss_file->streams[stream];
	if (substream == NULL)
		return -EINVAL;
	err = snd_pcm_oss_make_ready(substream);
	if (err < 0)
		return err;
	runtime = substream->runtime;
	if (runtime->oss.params || runtime->oss.prepare) {
		memset(&info, 0, sizeof(info));
		if (copy_to_user(_info, &info, sizeof(info)))
			return -EFAULT;
		return 0;
	}
	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
		err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DELAY, &delay);
		if (err == -EPIPE || err == -ESTRPIPE || (! err && delay < 0)) {
			err = 0;
			delay = 0;
			fixup = 0;
		} else {
			fixup = runtime->oss.buffer_used;
		}
	} else {
		err = snd_pcm_oss_capture_position_fixup(substream, &delay);
		fixup = -runtime->oss.buffer_used;
	}
	if (err < 0)
		return err;
	info.ptr = snd_pcm_oss_bytes(substream, runtime->status->hw_ptr % runtime->buffer_size);
	if (atomic_read(&substream->mmap_count)) {
		snd_pcm_sframes_t n;
		delay = get_hw_ptr_period(runtime);
		n = delay - runtime->oss.prev_hw_ptr_period;
		if (n < 0)
			n += runtime->boundary;
		info.blocks = n / runtime->period_size;
		runtime->oss.prev_hw_ptr_period = delay;
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			snd_pcm_oss_simulate_fill(substream, delay);
		info.bytes = snd_pcm_oss_bytes(substream, runtime->status->hw_ptr) & INT_MAX;
	} else {
		delay = snd_pcm_oss_bytes(substream, delay);
		if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
			if (substream->oss.setup.buggyptr)
				info.blocks = (runtime->oss.buffer_bytes - delay - fixup) / runtime->oss.period_bytes;
			else
				info.blocks = (delay + fixup) / runtime->oss.period_bytes;
			info.bytes = (runtime->oss.bytes - delay) & INT_MAX;
		} else {
			delay += fixup;
			info.blocks = delay / runtime->oss.period_bytes;
			info.bytes = (runtime->oss.bytes + delay) & INT_MAX;
		}
	}
	if (copy_to_user(_info, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}