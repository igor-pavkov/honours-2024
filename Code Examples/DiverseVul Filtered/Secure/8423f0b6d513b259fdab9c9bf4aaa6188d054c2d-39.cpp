static int snd_pcm_oss_post(struct snd_pcm_oss_file *pcm_oss_file)
{
	struct snd_pcm_substream *substream;
	int err;

	substream = pcm_oss_file->streams[SNDRV_PCM_STREAM_PLAYBACK];
	if (substream != NULL) {
		err = snd_pcm_oss_make_ready(substream);
		if (err < 0)
			return err;
		snd_pcm_kernel_ioctl(substream, SNDRV_PCM_IOCTL_START, NULL);
	}
	/* note: all errors from the start action are ignored */
	/* OSS apps do not know, how to handle them */
	return 0;
}