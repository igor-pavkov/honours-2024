R_API int r_bin_file_cur_set_plugin(RBinFile *binfile, RBinPlugin *plugin) {
	if (binfile && binfile->o) {
		binfile->o->plugin = plugin;
		return true;
	}
	return false;
}