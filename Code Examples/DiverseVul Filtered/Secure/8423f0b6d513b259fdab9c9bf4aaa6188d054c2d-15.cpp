static void snd_pcm_oss_simulate_fill(struct snd_pcm_substream *substream,
				      snd_pcm_uframes_t hw_ptr)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	snd_pcm_uframes_t appl_ptr;
	appl_ptr = hw_ptr + runtime->buffer_size;
	appl_ptr %= runtime->boundary;
	runtime->control->appl_ptr = appl_ptr;
}