static long snd_pcm_oss_bytes(struct snd_pcm_substream *substream, long frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	long buffer_size = snd_pcm_lib_buffer_bytes(substream);
	long bytes = frames_to_bytes(runtime, frames);
	if (buffer_size == runtime->oss.buffer_bytes)
		return bytes;
#if BITS_PER_LONG >= 64
	return runtime->oss.buffer_bytes * bytes / buffer_size;
#else
	{
		u64 bsize = (u64)runtime->oss.buffer_bytes * (u64)bytes;
		return div_u64(bsize, buffer_size);
	}
#endif
}