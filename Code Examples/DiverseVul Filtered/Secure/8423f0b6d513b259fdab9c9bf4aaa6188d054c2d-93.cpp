static int snd_pcm_oss_register_minor(struct snd_pcm *pcm)
{
	pcm->oss.reg = 0;
	if (dsp_map[pcm->card->number] == (int)pcm->device) {
		char name[128];
		int duplex;
		register_oss_dsp(pcm, 0);
		duplex = (pcm->streams[SNDRV_PCM_STREAM_PLAYBACK].substream_count > 0 && 
			      pcm->streams[SNDRV_PCM_STREAM_CAPTURE].substream_count && 
			      !(pcm->info_flags & SNDRV_PCM_INFO_HALF_DUPLEX));
		sprintf(name, "%s%s", pcm->name, duplex ? " (DUPLEX)" : "");
#ifdef SNDRV_OSS_INFO_DEV_AUDIO
		snd_oss_info_register(SNDRV_OSS_INFO_DEV_AUDIO,
				      pcm->card->number,
				      name);
#endif
		pcm->oss.reg++;
		pcm->oss.reg_mask |= 1;
	}
	if (adsp_map[pcm->card->number] == (int)pcm->device) {
		register_oss_dsp(pcm, 1);
		pcm->oss.reg++;
		pcm->oss.reg_mask |= 2;
	}

	if (pcm->oss.reg)
		snd_pcm_oss_proc_init(pcm);

	return 0;
}