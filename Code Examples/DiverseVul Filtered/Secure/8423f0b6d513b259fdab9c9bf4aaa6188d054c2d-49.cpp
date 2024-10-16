static int choose_rate(struct snd_pcm_substream *substream,
		       struct snd_pcm_hw_params *params, unsigned int best_rate)
{
	const struct snd_interval *it;
	struct snd_pcm_hw_params *save;
	unsigned int rate, prev;

	save = kmalloc(sizeof(*save), GFP_KERNEL);
	if (save == NULL)
		return -ENOMEM;
	*save = *params;
	it = hw_param_interval_c(save, SNDRV_PCM_HW_PARAM_RATE);

	/* try multiples of the best rate */
	rate = best_rate;
	for (;;) {
		if (it->max < rate || (it->max == rate && it->openmax))
			break;
		if (it->min < rate || (it->min == rate && !it->openmin)) {
			int ret;
			ret = snd_pcm_hw_param_set(substream, params,
						   SNDRV_PCM_HW_PARAM_RATE,
						   rate, 0);
			if (ret == (int)rate) {
				kfree(save);
				return rate;
			}
			*params = *save;
		}
		prev = rate;
		rate += best_rate;
		if (rate <= prev)
			break;
	}

	/* not found, use the nearest rate */
	kfree(save);
	return snd_pcm_hw_param_near(substream, params, SNDRV_PCM_HW_PARAM_RATE, best_rate, NULL);
}