R_IPI RBinFile *r_bin_file_new(RBin *bin, const char *file, ut64 file_sz, int rawstr, int fd, const char *xtrname, Sdb *sdb, bool steal_ptr) {
	ut32 bf_id;
	if (!r_id_pool_grab_id (bin->ids->pool, &bf_id)) {
		return NULL;
	}
	RBinFile *bf = R_NEW0 (RBinFile);
	if (bf) {
		bf->id = bf_id;
		bf->rbin = bin;
		bf->file = file ? strdup (file) : NULL;
		bf->rawstr = rawstr;
		bf->fd = fd;
		bf->curxtr = xtrname ? r_bin_get_xtrplugin_by_name (bin, xtrname) : NULL;
		bf->sdb = sdb;
		bf->size = file_sz;
		bf->xtr_data = r_list_newf ((RListFree)r_bin_xtrdata_free);
		bf->xtr_obj = NULL;
		bf->sdb = sdb_new0 ();
		bf->sdb_addrinfo = sdb_new0 (); //ns (bf->sdb, "addrinfo", 1);
		// bf->sdb_addrinfo->refs++;
	}
	return bf;
}