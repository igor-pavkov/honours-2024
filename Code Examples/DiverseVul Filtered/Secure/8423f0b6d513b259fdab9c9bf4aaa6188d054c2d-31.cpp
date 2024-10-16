int snd_pcm_plugin_append(struct snd_pcm_plugin *plugin)
{
	struct snd_pcm_runtime *runtime = plugin->plug->runtime;
	plugin->next = NULL;
	plugin->prev = runtime->oss.plugin_last;
	if (runtime->oss.plugin_last) {
		runtime->oss.plugin_last->next = plugin;
		runtime->oss.plugin_last = plugin;
	} else {
		runtime->oss.plugin_last =
		runtime->oss.plugin_first = plugin;
	}
	return 0;
}