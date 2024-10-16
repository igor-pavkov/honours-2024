static int snd_pcm_hw_param_set(struct snd_pcm_substream *pcm,
				struct snd_pcm_hw_params *params,
				snd_pcm_hw_param_t var, unsigned int val,
				int dir)
{
	int changed = _snd_pcm_hw_param_set(params, var, val, dir);
	if (changed < 0)
		return changed;
	if (params->rmask) {
		int err = snd_pcm_hw_refine(pcm, params);
		if (err < 0)
			return err;
	}
	return snd_pcm_hw_param_value(params, var, NULL);
}