static int snd_pcm_plugin_insert(struct snd_pcm_plugin *plugin)
{
	struct snd_pcm_runtime *runtime = plugin->plug->runtime;
	plugin->next = runtime->oss.plugin_first;
	plugin->prev = NULL;
	if (runtime->oss.plugin_first) {
		runtime->oss.plugin_first->prev = plugin;
		runtime->oss.plugin_first = plugin;
	} else {
		runtime->oss.plugin_last =
		runtime->oss.plugin_first = plugin;
	}
	return 0;
}