R_IPI RBinFile *r_bin_file_new_from_buffer(RBin *bin, const char *file, RBuffer *buf, int rawstr, ut64 baseaddr, ut64 loadaddr, int fd, const char *pluginname) {
	r_return_val_if_fail (bin && file && buf, NULL);

	RBinFile *bf = r_bin_file_new (bin, file, r_buf_size (buf), rawstr, fd, pluginname, NULL, false);
	if (bf) {
		RListIter *item = r_list_append (bin->binfiles, bf);
		bf->buf = r_buf_ref (buf);
		RBinPlugin *plugin = get_plugin_from_buffer (bin, bf, pluginname, bf->buf);
		RBinObject *o = r_bin_object_new (bf, plugin, baseaddr, loadaddr, 0, r_buf_size (bf->buf));
		if (!o) {
			r_list_delete (bin->binfiles, item);
			return NULL;
		}
		// size is set here because the reported size of the object depends on
		// if loaded from xtr plugin or partially read
		if (!o->size) {
			o->size = r_buf_size (buf);
		}
	}
	return bf;
}