static RBinPlugin *r_bin_get_binplugin_by_name(RBin *bin, const char *name) {
	RBinPlugin *plugin;
	RListIter *it;
	if (bin && name) {
		r_list_foreach (bin->plugins, it, plugin) {
			if (!strcmp (plugin->name, name)) {
				return plugin;
			}
		}
	}
	return NULL;
}