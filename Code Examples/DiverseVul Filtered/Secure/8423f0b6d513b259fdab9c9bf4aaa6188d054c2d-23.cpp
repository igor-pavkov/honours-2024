static void snd_pcm_oss_init_substream(struct snd_pcm_substream *substream,
				       struct snd_pcm_oss_setup *setup,
				       int minor)
{
	struct snd_pcm_runtime *runtime;

	substream->oss.oss = 1;
	substream->oss.setup = *setup;
	if (setup->nonblock)
		substream->f_flags |= O_NONBLOCK;
	else if (setup->block)
		substream->f_flags &= ~O_NONBLOCK;
	runtime = substream->runtime;
	runtime->oss.params = 1;
	runtime->oss.trigger = 1;
	runtime->oss.rate = 8000;
	mutex_init(&runtime->oss.params_lock);
	switch (SNDRV_MINOR_OSS_DEVICE(minor)) {
	case SNDRV_MINOR_OSS_PCM_8:
		runtime->oss.format = AFMT_U8;
		break;
	case SNDRV_MINOR_OSS_PCM_16:
		runtime->oss.format = AFMT_S16_LE;
		break;
	default:
		runtime->oss.format = AFMT_MU_LAW;
	}
	runtime->oss.channels = 1;
	runtime->oss.fragshift = 0;
	runtime->oss.maxfrags = 0;
	runtime->oss.subdivision = 0;
	substream->pcm_release = snd_pcm_oss_release_substream;
	atomic_set(&runtime->oss.rw_ref, 0);
}