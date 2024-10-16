static int snd_pcm_hw_param_mask(struct snd_pcm_substream *pcm,
				 struct snd_pcm_hw_params *params,
				 snd_pcm_hw_param_t var,
				 const struct snd_mask *val)
{
	int changed = _snd_pcm_hw_param_mask(params, var, val);
	if (changed < 0)
		return changed;
	if (params->rmask) {
		int err = snd_pcm_hw_refine(pcm, params);
		if (err < 0)
			return err;
	}
	return 0;
}