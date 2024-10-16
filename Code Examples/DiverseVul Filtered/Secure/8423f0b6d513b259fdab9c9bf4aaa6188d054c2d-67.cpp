static void snd_pcm_oss_release_buffers(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	kvfree(runtime->oss.buffer);
	runtime->oss.buffer = NULL;
#ifdef CONFIG_SND_PCM_OSS_PLUGINS
	snd_pcm_oss_plugin_clear(substream);
#endif
}