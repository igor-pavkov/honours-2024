R_API RBinFile *r_bin_file_new_from_fd(RBin *bin, int fd, RBinFileOptions *options) {
int file_sz = 0;
	RBinPlugin *plugin = NULL;
	RBinFile *bf = r_bin_file_create_append (bin, "-", NULL, 0, file_sz,
				       0, fd, NULL, false);
	if (!bf) {
		return NULL;
	}
	int loadaddr = options? options->laddr: 0;
	int baseaddr = options? options->baddr: 0;
	// int loadaddr = options? options->laddr: 0;
	bool binfile_created = true;
	r_buf_free (bf->buf);
	bf->buf = r_buf_new_with_io (&bin->iob, fd);
	if (bin->force) {
		plugin = r_bin_get_binplugin_by_name (bin, bin->force);
	}
	if (!plugin) {
		if (options && options->plugname) {
			plugin = r_bin_get_binplugin_by_name (bin, options->plugname);
		}
		if (!plugin) {
			ut8 bytes[1024];
			int sz = 1024;
			r_buf_read_at (bf->buf, 0, bytes, sz);
			plugin = r_bin_get_binplugin_by_bytes (bin, bytes, sz);
			if (!plugin) {
				plugin = r_bin_get_binplugin_any (bin);
			}
		}
	}

	RBinObject *o = r_bin_object_new (bf, plugin, baseaddr, loadaddr, 0, r_buf_size (bf->buf));
	// size is set here because the reported size of the object depends on
	// if loaded from xtr plugin or partially read
	if (o && !o->size) {
		o->size = file_sz;
	}

	if (!o) {
		if (bf && binfile_created) {
			r_list_delete_data (bin->binfiles, bf);
		}
		return NULL;
	}
#if 0
	/* WTF */
	if (strcmp (plugin->name, "any")) {
		bf->narch = 1;
	}
#endif
	/* free unnecessary rbuffer (???) */
	return bf;
}