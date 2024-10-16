static int _snd_pcm_hw_param_setinteger(struct snd_pcm_hw_params *params,
					snd_pcm_hw_param_t var)
{
	int changed;
	changed = snd_interval_setinteger(hw_param_interval(params, var));
	if (changed > 0) {
		params->cmask |= 1 << var;
		params->rmask |= 1 << var;
	}
	return changed;
}