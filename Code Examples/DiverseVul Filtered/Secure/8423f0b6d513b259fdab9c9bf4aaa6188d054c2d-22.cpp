static int snd_pcm_oss_get_formats(struct snd_pcm_oss_file *pcm_oss_file)
{
	struct snd_pcm_substream *substream;
	int err;
	int direct;
	struct snd_pcm_hw_params *params;
	unsigned int formats = 0;
	const struct snd_mask *format_mask;
	int fmt;

	err = snd_pcm_oss_get_active_substream(pcm_oss_file, &substream);
	if (err < 0)
		return err;
	if (atomic_read(&substream->mmap_count))
		direct = 1;
	else
		direct = substream->oss.setup.direct;
	if (!direct)
		return AFMT_MU_LAW | AFMT_U8 |
		       AFMT_S16_LE | AFMT_S16_BE |
		       AFMT_S8 | AFMT_U16_LE |
		       AFMT_U16_BE |
			AFMT_S32_LE | AFMT_S32_BE |
			AFMT_S24_LE | AFMT_S24_BE |
			AFMT_S24_PACKED;
	params = kmalloc(sizeof(*params), GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	_snd_pcm_hw_params_any(params);
	err = snd_pcm_hw_refine(substream, params);
	if (err < 0)
		goto error;
	format_mask = hw_param_mask_c(params, SNDRV_PCM_HW_PARAM_FORMAT);
	for (fmt = 0; fmt < 32; ++fmt) {
		if (snd_mask_test(format_mask, fmt)) {
			int f = snd_pcm_oss_format_to((__force snd_pcm_format_t)fmt);
			if (f >= 0)
				formats |= f;
		}
	}

 error:
	kfree(params);
	return err < 0 ? err : formats;
}