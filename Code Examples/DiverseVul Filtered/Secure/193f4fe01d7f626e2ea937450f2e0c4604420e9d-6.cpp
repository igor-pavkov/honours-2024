R_IPI bool r_bin_file_set_obj(RBin *bin, RBinFile *bf, RBinObject *obj) {
	r_return_val_if_fail (bin && bf, false);
	bin->file = bf->file;
	bin->cur = bf;
	bin->narch = bf->narch;
	if (obj) {
		bf->o = obj;
	} else {
		obj = bf->o;
	}
	RBinPlugin *plugin = r_bin_file_cur_plugin (bf);
	if (bin->minstrlen < 1) {
		bin->minstrlen = plugin? plugin->minstrlen: bin->minstrlen;
	}
	if (obj) {
		if (!obj->info) {
			return false;
		}
		if (!obj->info->lang) {
			obj->info->lang = r_bin_lang_tostring (obj->lang);
		}
	}
	return true;
}