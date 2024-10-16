static int snd_pcm_oss_disconnect_minor(struct snd_pcm *pcm)
{
	if (pcm->oss.reg) {
		if (pcm->oss.reg_mask & 1) {
			pcm->oss.reg_mask &= ~1;
			snd_unregister_oss_device(SNDRV_OSS_DEVICE_TYPE_PCM,
						  pcm->card, 0);
		}
		if (pcm->oss.reg_mask & 2) {
			pcm->oss.reg_mask &= ~2;
			snd_unregister_oss_device(SNDRV_OSS_DEVICE_TYPE_PCM,
						  pcm->card, 1);
		}
		if (dsp_map[pcm->card->number] == (int)pcm->device) {
#ifdef SNDRV_OSS_INFO_DEV_AUDIO
			snd_oss_info_unregister(SNDRV_OSS_INFO_DEV_AUDIO, pcm->card->number);
#endif
		}
		pcm->oss.reg = 0;
	}
	return 0;
}