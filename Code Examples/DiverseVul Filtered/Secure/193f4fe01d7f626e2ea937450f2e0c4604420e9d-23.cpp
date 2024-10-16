static RBinPlugin *get_plugin_from_buffer(RBin *bin, RBinFile *bf, const char *pluginname, RBuffer *buf) {
	RBinPlugin *plugin = bin->force? r_bin_get_binplugin_by_name (bin, bin->force): NULL;
	if (plugin) {
		return plugin;
	}
	plugin = pluginname? r_bin_get_binplugin_by_name (bin, pluginname): NULL;
	if (plugin) {
		return plugin;
	}
	plugin = r_bin_get_binplugin_by_buffer (bin, bf, buf);
	if (plugin) {
		return plugin;
	}
	return r_bin_get_binplugin_by_name (bin, "any");
}