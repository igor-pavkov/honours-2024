static int snd_pcm_oss_plugin_clear(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_pcm_plugin *plugin, *next;
	
	plugin = runtime->oss.plugin_first;
	while (plugin) {
		next = plugin->next;
		snd_pcm_plugin_free(plugin);
		plugin = next;
	}
	runtime->oss.plugin_first = runtime->oss.plugin_last = NULL;
	return 0;
}