R_API int r_bin_file_set_bytes(RBinFile *binfile, const ut8 *bytes, ut64 sz, bool steal_ptr) {
	if (!binfile) {
		return false;
	}
	if (!bytes) {
		return false;
	}
	r_buf_free (binfile->buf);
	binfile->buf = r_buf_new ();
#if LIMIT_SIZE
	if (sz > 1024 * 1024) {
		eprintf ("Too big\n");
		// TODO: use r_buf_io instead of setbytes all the time to save memory
		return NULL;
	}
#else
	if (steal_ptr) {
		r_buf_set_bytes_steal (binfile->buf, bytes, sz);
	} else {
		r_buf_set_bytes (binfile->buf, bytes, sz);
	}
#endif
	return binfile->buf != NULL;
}