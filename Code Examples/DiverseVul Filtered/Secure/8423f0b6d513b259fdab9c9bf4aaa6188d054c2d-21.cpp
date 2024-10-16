snd_pcm_uframes_t get_hw_ptr_period(struct snd_pcm_runtime *runtime)
{
	return runtime->hw_ptr_interrupt;
}