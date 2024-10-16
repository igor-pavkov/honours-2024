static int _snd_pcm_hw_param_mask(struct snd_pcm_hw_params *params,
				  snd_pcm_hw_param_t var,
				  const struct snd_mask *val)
{
	int changed;
	changed = snd_mask_refine(hw_param_mask(params, var), val);
	if (changed > 0) {
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
	}
	return changed;
}