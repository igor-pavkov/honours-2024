R_API RList *r_bin_file_get_trycatch(RBinFile *bf) {
	r_return_val_if_fail (bf && bf->o && bf->o->plugin, NULL);
	if (bf->o->plugin->trycatch) {
		return bf->o->plugin->trycatch (bf);
	}
	return NULL;
}