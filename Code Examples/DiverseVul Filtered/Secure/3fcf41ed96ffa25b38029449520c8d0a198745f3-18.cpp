R_API RBinPlugin *r_bin_file_cur_plugin(RBinFile *binfile) {
	return binfile && binfile->o? binfile->o->plugin: NULL;
}