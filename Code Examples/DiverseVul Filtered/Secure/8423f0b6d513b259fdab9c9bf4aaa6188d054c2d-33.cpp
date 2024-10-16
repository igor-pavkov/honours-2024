static long snd_pcm_alsa_frames(struct snd_pcm_substream *substream, long bytes)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	long buffer_size = snd_pcm_lib_buffer_bytes(substream);
	if (buffer_size == runtime->oss.buffer_bytes)
		return bytes_to_frames(runtime, bytes);
	return bytes_to_frames(runtime, (buffer_size * bytes) / runtime->oss.buffer_bytes);
}