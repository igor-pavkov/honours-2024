static int snd_pcm_oss_get_space(struct snd_pcm_oss_file *pcm_oss_file, int stream, struct audio_buf_info __user *_info)
{
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;
	snd_pcm_sframes_t avail;
	int fixup;
	struct audio_buf_info info;
	int err;

	if (_info == NULL)
		return -EFAULT;
	substream = pcm_oss_file->streams[stream];
	if (substream == NULL)
		return -EINVAL;
	runtime = substream->runtime;

	if (runtime->oss.params) {
		err = snd_pcm_oss_change_params(substream, false);
		if (err < 0)
			return err;
	}

	info.fragsize = runtime->oss.period_bytes;
	info.fragstotal = runtime->periods;
	if (runtime->oss.prepare) {
		if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
			info.bytes = runtime->oss.period_bytes * runtime->oss.periods;
			info.fragments = runtime->oss.periods;
		} else {
			info.bytes = 0;
			info.fragments = 0;
		}
	} else {
		if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
			err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DELAY, &avail);
			if (err == -EPIPE || err == -ESTRPIPE || (! err && avail < 0)) {
				avail = runtime->buffer_size;
				err = 0;
				fixup = 0;
			} else {
				avail = runtime->buffer_size - avail;
				fixup = -runtime->oss.buffer_used;
			}
		} else {
			err = snd_pcm_oss_capture_position_fixup(substream, &avail);
			fixup = runtime->oss.buffer_used;
		}
		if (err < 0)
			return err;
		info.bytes = snd_pcm_oss_bytes(substream, avail) + fixup;
		info.fragments = info.bytes / runtime->oss.period_bytes;
	}

#ifdef OSS_DEBUG
	pcm_dbg(substream->pcm,
		"pcm_oss: space: bytes = %i, fragments = %i, fragstotal = %i, fragsize = %i\n",
		info.bytes, info.fragments, info.fragstotal, info.fragsize);
#endif
	if (copy_to_user(_info, &info, sizeof(info)))
		return -EFAULT;
	return 0;
}