R_API bool r_bin_file_set_cur_binfile_obj(RBin *bin, RBinFile *bf, RBinObject *obj) {
	RBinPlugin *plugin = NULL;
	if (!bin || !bf || !obj) {
		return false;
	}
	bin->file = bf->file;
	bin->cur = bf;
	bin->narch = bf->narch;
	bf->o = obj;
	plugin = r_bin_file_cur_plugin (bf);
	if (bin->minstrlen < 1) {
		bin->minstrlen = plugin? plugin->minstrlen: bin->minstrlen;
	}
	return true;
}