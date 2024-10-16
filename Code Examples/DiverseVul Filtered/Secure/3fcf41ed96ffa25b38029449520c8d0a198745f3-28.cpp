R_API RBinFile *r_bin_file_new(RBin *bin, const char *file, const ut8 *bytes, ut64 sz, ut64 file_sz, int rawstr, int fd, const char *xtrname, Sdb *sdb, bool steal_ptr) {
	RBinFile *binfile = R_NEW0 (RBinFile);
	if (!binfile) {
		return NULL;
	}
	if (!r_id_pool_grab_id (bin->file_ids, &binfile->id)) {
		if (steal_ptr) { // we own the ptr, free on error
			free ((void*) bytes);
		}
		free (binfile);		//no id means no binfile
		return NULL;
	}
	int res = r_bin_file_set_bytes (binfile, bytes, sz, steal_ptr);
	if (!res && steal_ptr) { // we own the ptr, free on error
		free((void*) bytes);
	}
	binfile->rbin = bin;
	binfile->file = file? strdup (file): NULL;
	binfile->rawstr = rawstr;
	binfile->fd = fd;
	binfile->curxtr = r_bin_get_xtrplugin_by_name (bin, xtrname);
	binfile->sdb = sdb;
	binfile->size = file_sz;
	binfile->xtr_data = r_list_newf ((RListFree)r_bin_xtrdata_free);
	binfile->objs = r_list_newf ((RListFree)r_bin_object_free);
	binfile->xtr_obj  = NULL;

	if (!binfile->buf) {
		//r_bin_file_free (binfile);
		binfile->buf = r_buf_new ();
		//	return NULL;
	}

	if (sdb) {
		binfile->sdb = sdb_ns (sdb, sdb_fmt ("fd.%d", fd), 1);
		sdb_set (binfile->sdb, "archs", "0:0:x86:32", 0); // x86??
		/* NOTE */
		/* Those refs++ are necessary because sdb_ns() doesnt rerefs all
		 * sub-namespaces */
		/* And if any namespace is referenced backwards it gets
		 * double-freed */
		binfile->sdb_addrinfo = sdb_ns (binfile->sdb, "addrinfo", 1);
		binfile->sdb_addrinfo->refs++;
		sdb_ns_set (sdb, "cur", binfile->sdb);
		binfile->sdb->refs++;
	}
	return binfile;
}