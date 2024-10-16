R_API RBinFile *r_bin_file_new_from_bytes(RBin *bin, const char *file, const ut8 *bytes, ut64 sz, ut64 file_sz, int rawstr, ut64 baseaddr, ut64 loadaddr, int fd, const char *pluginname, const char *xtrname, ut64 offset, bool steal_ptr) {
	ut8 binfile_created = false;
	RBinPlugin *plugin = NULL;
	RBinXtrPlugin *xtr = NULL;
	RBinObject *o = NULL;
	if (sz == UT64_MAX) {
		return NULL;
	}

	if (xtrname) {
		xtr = r_bin_get_xtrplugin_by_name (bin, xtrname);
	}

	if (xtr && xtr->check_bytes (bytes, sz)) {
		return r_bin_file_xtr_load_bytes (bin, xtr, file,
						bytes, sz, file_sz, baseaddr, loadaddr, 0,
						fd, rawstr);
	}

	RBinFile *bf = r_bin_file_create_append (bin, file, bytes, sz, file_sz,
				       rawstr, fd, xtrname, steal_ptr);
	if (!bf) {
		if (!steal_ptr) { // we own the ptr, free on error
			free ((void*) bytes);
		}
		return NULL;
	}
	binfile_created = true;

	if (bin->force) {
		plugin = r_bin_get_binplugin_by_name (bin, bin->force);
	}
	if (!plugin) {
		if (pluginname) {
			plugin = r_bin_get_binplugin_by_name (bin, pluginname);
		}
		if (!plugin) {
			plugin = r_bin_get_binplugin_by_bytes (bin, bytes, sz);
			if (!plugin) {
				plugin = r_bin_get_binplugin_any (bin);
			}
		}
	}

	o = r_bin_object_new (bf, plugin, baseaddr, loadaddr, 0, r_buf_size (bf->buf));
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