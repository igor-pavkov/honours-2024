static int snd_pcm_oss_playback_ready(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	if (atomic_read(&substream->mmap_count))
		return runtime->oss.prev_hw_ptr_period !=
						get_hw_ptr_period(runtime);
	else
		return snd_pcm_playback_avail(runtime) >=
						runtime->oss.period_frames;
}