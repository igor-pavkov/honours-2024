static int snd_pcm_oss_change_params_locked(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_hw_params *params, *sparams;
	struct snd_pcm_sw_params *sw_params;
	ssize_t oss_buffer_size, oss_period_size;
	size_t oss_frame_size;
	int err;
	int direct;
	snd_pcm_format_t format, sformat;
	int n;
	const struct snd_mask *sformat_mask;
	struct snd_mask mask;

	if (!runtime->oss.params)
		return 0;
	sw_params = kzalloc(sizeof(*sw_params), GFP_KERNEL);
	params = kmalloc(sizeof(*params), GFP_KERNEL);
	sparams = kmalloc(sizeof(*sparams), GFP_KERNEL);
	if (!sw_params || !params || !sparams) {
		err = -ENOMEM;
		goto failure;
	}

	if (atomic_read(&substream->mmap_count))
		direct = 1;
	else
		direct = substream->oss.setup.direct;

	_snd_pcm_hw_params_any(sparams);
	_snd_pcm_hw_param_setinteger(sparams, SNDRV_PCM_HW_PARAM_PERIODS);
	_snd_pcm_hw_param_min(sparams, SNDRV_PCM_HW_PARAM_PERIODS, 2, 0);
	snd_mask_none(&mask);
	if (atomic_read(&substream->mmap_count))
		snd_mask_set(&mask, (__force int)SNDRV_PCM_ACCESS_MMAP_INTERLEAVED);
	else {
		snd_mask_set(&mask, (__force int)SNDRV_PCM_ACCESS_RW_INTERLEAVED);
		if (!direct)
			snd_mask_set(&mask, (__force int)SNDRV_PCM_ACCESS_RW_NONINTERLEAVED);
	}
	err = snd_pcm_hw_param_mask(substream, sparams, SNDRV_PCM_HW_PARAM_ACCESS, &mask);
	if (err < 0) {
		pcm_dbg(substream->pcm, "No usable accesses\n");
		err = -EINVAL;
		goto failure;
	}

	err = choose_rate(substream, sparams, runtime->oss.rate);
	if (err < 0)
		goto failure;
	err = snd_pcm_hw_param_near(substream, sparams,
				    SNDRV_PCM_HW_PARAM_CHANNELS,
				    runtime->oss.channels, NULL);
	if (err < 0)
		goto failure;

	format = snd_pcm_oss_format_from(runtime->oss.format);

	sformat_mask = hw_param_mask_c(sparams, SNDRV_PCM_HW_PARAM_FORMAT);
	if (direct)
		sformat = format;
	else
		sformat = snd_pcm_plug_slave_format(format, sformat_mask);

	if ((__force int)sformat < 0 ||
	    !snd_mask_test_format(sformat_mask, sformat)) {
		pcm_for_each_format(sformat) {
			if (snd_mask_test_format(sformat_mask, sformat) &&
			    snd_pcm_oss_format_to(sformat) >= 0)
				goto format_found;
		}
		pcm_dbg(substream->pcm, "Cannot find a format!!!\n");
		err = -EINVAL;
		goto failure;
	}
 format_found:
	err = _snd_pcm_hw_param_set(sparams, SNDRV_PCM_HW_PARAM_FORMAT, (__force int)sformat, 0);
	if (err < 0)
		goto failure;

	if (direct) {
		memcpy(params, sparams, sizeof(*params));
	} else {
		_snd_pcm_hw_params_any(params);
		_snd_pcm_hw_param_set(params, SNDRV_PCM_HW_PARAM_ACCESS,
				      (__force int)SNDRV_PCM_ACCESS_RW_INTERLEAVED, 0);
		_snd_pcm_hw_param_set(params, SNDRV_PCM_HW_PARAM_FORMAT,
				      (__force int)snd_pcm_oss_format_from(runtime->oss.format), 0);
		_snd_pcm_hw_param_set(params, SNDRV_PCM_HW_PARAM_CHANNELS,
				      runtime->oss.channels, 0);
		_snd_pcm_hw_param_set(params, SNDRV_PCM_HW_PARAM_RATE,
				      runtime->oss.rate, 0);
		pdprintf("client: access = %i, format = %i, channels = %i, rate = %i\n",
			 params_access(params), params_format(params),
			 params_channels(params), params_rate(params));
	}
	pdprintf("slave: access = %i, format = %i, channels = %i, rate = %i\n",
		 params_access(sparams), params_format(sparams),
		 params_channels(sparams), params_rate(sparams));

	oss_frame_size = snd_pcm_format_physical_width(params_format(params)) *
			 params_channels(params) / 8;

	err = snd_pcm_oss_period_size(substream, params, sparams);
	if (err < 0)
		goto failure;

	n = snd_pcm_plug_slave_size(substream, runtime->oss.period_bytes / oss_frame_size);
	err = snd_pcm_hw_param_near(substream, sparams, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, n, NULL);
	if (err < 0)
		goto failure;

	err = snd_pcm_hw_param_near(substream, sparams, SNDRV_PCM_HW_PARAM_PERIODS,
				     runtime->oss.periods, NULL);
	if (err < 0)
		goto failure;

	snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_DROP, NULL);

	err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_HW_PARAMS, sparams);
	if (err < 0) {
		pcm_dbg(substream->pcm, "HW_PARAMS failed: %i\n", err);
		goto failure;
	}

#ifdef CONFIG_SND_PCM_OSS_PLUGINS
	snd_pcm_oss_plugin_clear(substream);
	if (!direct) {
		/* add necessary plugins */
		err = snd_pcm_plug_format_plugins(substream, params, sparams);
		if (err < 0) {
			pcm_dbg(substream->pcm,
				"snd_pcm_plug_format_plugins failed: %i\n", err);
			goto failure;
		}
		if (runtime->oss.plugin_first) {
			struct snd_pcm_plugin *plugin;
			err = snd_pcm_plugin_build_io(substream, sparams, &plugin);
			if (err < 0) {
				pcm_dbg(substream->pcm,
					"snd_pcm_plugin_build_io failed: %i\n", err);
				goto failure;
			}
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
				err = snd_pcm_plugin_append(plugin);
			} else {
				err = snd_pcm_plugin_insert(plugin);
			}
			if (err < 0)
				goto failure;
		}
	}
#endif

	if (runtime->oss.trigger) {
		sw_params->start_threshold = 1;
	} else {
		sw_params->start_threshold = runtime->boundary;
	}
	if (atomic_read(&substream->mmap_count) ||
	    substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		sw_params->stop_threshold = runtime->boundary;
	else
		sw_params->stop_threshold = runtime->buffer_size;
	sw_params->tstamp_mode = SNDRV_PCM_TSTAMP_NONE;
	sw_params->period_step = 1;
	sw_params->avail_min = substream->stream == SNDRV_PCM_STREAM_PLAYBACK ?
		1 : runtime->period_size;
	if (atomic_read(&substream->mmap_count) ||
	    substream->oss.setup.nosilence) {
		sw_params->silence_threshold = 0;
		sw_params->silence_size = 0;
	} else {
		snd_pcm_uframes_t frames;
		frames = runtime->period_size + 16;
		if (frames > runtime->buffer_size)
			frames = runtime->buffer_size;
		sw_params->silence_threshold = frames;
		sw_params->silence_size = frames;
	}

	err = snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_SW_PARAMS, sw_params);
	if (err < 0) {
		pcm_dbg(substream->pcm, "SW_PARAMS failed: %i\n", err);
		goto failure;
	}

	runtime->oss.periods = params_periods(sparams);
	oss_period_size = snd_pcm_plug_client_size(substream, params_period_size(sparams));
	if (oss_period_size < 0) {
		err = -EINVAL;
		goto failure;
	}
#ifdef CONFIG_SND_PCM_OSS_PLUGINS
	if (runtime->oss.plugin_first) {
		err = snd_pcm_plug_alloc(substream, oss_period_size);
		if (err < 0)
			goto failure;
	}
#endif
	oss_period_size = array_size(oss_period_size, oss_frame_size);
	oss_buffer_size = array_size(oss_period_size, runtime->oss.periods);
	if (oss_buffer_size <= 0) {
		err = -EINVAL;
		goto failure;
	}

	runtime->oss.period_bytes = oss_period_size;
	runtime->oss.buffer_bytes = oss_buffer_size;

	pdprintf("oss: period bytes = %i, buffer bytes = %i\n",
		 runtime->oss.period_bytes,
		 runtime->oss.buffer_bytes);
	pdprintf("slave: period_size = %i, buffer_size = %i\n",
		 params_period_size(sparams),
		 params_buffer_size(sparams));

	runtime->oss.format = snd_pcm_oss_format_to(params_format(params));
	runtime->oss.channels = params_channels(params);
	runtime->oss.rate = params_rate(params);

	kvfree(runtime->oss.buffer);
	runtime->oss.buffer = kvzalloc(runtime->oss.period_bytes, GFP_KERNEL);
	if (!runtime->oss.buffer) {
		err = -ENOMEM;
		goto failure;
	}

	runtime->oss.params = 0;
	runtime->oss.prepare = 1;
	runtime->oss.buffer_used = 0;
	if (runtime->dma_area)
		snd_pcm_format_set_silence(runtime->format, runtime->dma_area, bytes_to_samples(runtime, runtime->dma_bytes));

	runtime->oss.period_frames = snd_pcm_alsa_frames(substream, oss_period_size);

	err = 0;
failure:
	if (err)
		snd_pcm_oss_release_buffers(substream);
	kfree(sw_params);
	kfree(params);
	kfree(sparams);
	return err;
}