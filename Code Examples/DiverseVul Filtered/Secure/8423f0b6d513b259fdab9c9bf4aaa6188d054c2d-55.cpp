static int __init alsa_pcm_oss_init(void)
{
	int i;
	int err;

	/* check device map table */
	for (i = 0; i < SNDRV_CARDS; i++) {
		if (dsp_map[i] < 0 || dsp_map[i] >= SNDRV_PCM_DEVICES) {
			pr_err("ALSA: pcm_oss: invalid dsp_map[%d] = %d\n",
				   i, dsp_map[i]);
			dsp_map[i] = 0;
		}
		if (adsp_map[i] < 0 || adsp_map[i] >= SNDRV_PCM_DEVICES) {
			pr_err("ALSA: pcm_oss: invalid adsp_map[%d] = %d\n",
				   i, adsp_map[i]);
			adsp_map[i] = 1;
		}
	}
	err = snd_pcm_notify(&snd_pcm_oss_notify, 0);
	if (err < 0)
		return err;
	return 0;
}