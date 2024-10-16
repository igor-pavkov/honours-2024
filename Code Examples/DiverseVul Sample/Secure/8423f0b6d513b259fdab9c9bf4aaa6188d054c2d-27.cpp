static int snd_pcm_hw_param_near(struct snd_pcm_substream *pcm,
				 struct snd_pcm_hw_params *params,
				 snd_pcm_hw_param_t var, unsigned int best,
				 int *dir)
{
	struct snd_pcm_hw_params *save = NULL;
	int v;
	unsigned int saved_min;
	int last = 0;
	int min, max;
	int mindir, maxdir;
	int valdir = dir ? *dir : 0;
	/* FIXME */
	if (best > INT_MAX)
		best = INT_MAX;
	min = max = best;
	mindir = maxdir = valdir;
	if (maxdir > 0)
		maxdir = 0;
	else if (maxdir == 0)
		maxdir = -1;
	else {
		maxdir = 1;
		max--;
	}
	save = kmalloc(sizeof(*save), GFP_KERNEL);
	if (save == NULL)
		return -ENOMEM;
	*save = *params;
	saved_min = min;
	min = snd_pcm_hw_param_min(pcm, params, var, min, &mindir);
	if (min >= 0) {
		struct snd_pcm_hw_params *params1;
		if (max < 0)
			goto _end;
		if ((unsigned int)min == saved_min && mindir == valdir)
			goto _end;
		params1 = kmalloc(sizeof(*params1), GFP_KERNEL);
		if (params1 == NULL) {
			kfree(save);
			return -ENOMEM;
		}
		*params1 = *save;
		max = snd_pcm_hw_param_max(pcm, params1, var, max, &maxdir);
		if (max < 0) {
			kfree(params1);
			goto _end;
		}
		if (boundary_nearer(max, maxdir, best, valdir, min, mindir)) {
			*params = *params1;
			last = 1;
		}
		kfree(params1);
	} else {
		*params = *save;
		max = snd_pcm_hw_param_max(pcm, params, var, max, &maxdir);
		if (max < 0) {
			kfree(save);
			return max;
		}
		last = 1;
	}
 _end:
 	kfree(save);
	if (last)
		v = snd_pcm_hw_param_last(pcm, params, var, dir);
	else
		v = snd_pcm_hw_param_first(pcm, params, var, dir);
	return v;
}