static int snd_pcm_oss_set_fragment1(struct snd_pcm_substream *substream, unsigned int val)
{
	struct snd_pcm_runtime *runtime;
	int fragshift;

	runtime = substream->runtime;
	if (runtime->oss.subdivision || runtime->oss.fragshift)
		return -EINVAL;
	fragshift = val & 0xffff;
	if (fragshift >= 25) /* should be large enough */
		return -EINVAL;
	runtime->oss.fragshift = fragshift;
	runtime->oss.maxfrags = (val >> 16) & 0xffff;
	if (runtime->oss.fragshift < 4)		/* < 16 */
		runtime->oss.fragshift = 4;
	if (runtime->oss.maxfrags < 2)
		runtime->oss.maxfrags = 2;
	runtime->oss.params = 1;
	return 0;
}