R_API RBinPlugin *r_bin_file_cur_plugin(RBinFile *bf) {
	return (bf && bf->o)? bf->o->plugin: NULL;
}