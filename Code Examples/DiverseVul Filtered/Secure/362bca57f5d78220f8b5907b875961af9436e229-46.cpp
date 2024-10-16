static int snd_pcm_free(struct snd_pcm *pcm)
{
	if (!pcm)
		return 0;
	if (!pcm->internal)
		pcm_call_notify(pcm, n_unregister);
	if (pcm->private_free)
		pcm->private_free(pcm);
	snd_pcm_lib_preallocate_free_for_all(pcm);
	snd_pcm_free_stream(&pcm->streams[SNDRV_PCM_STREAM_PLAYBACK]);
	snd_pcm_free_stream(&pcm->streams[SNDRV_PCM_STREAM_CAPTURE]);
	kfree(pcm);
	return 0;
}