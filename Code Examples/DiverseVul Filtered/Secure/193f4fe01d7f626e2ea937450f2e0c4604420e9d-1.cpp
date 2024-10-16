R_IPI bool r_bin_file_set_bytes(RBinFile *bf, const ut8 *bytes, ut64 sz, bool steal_ptr) {
	r_return_val_if_fail (bf && bytes, false);
	r_buf_free (bf->buf);
	if (steal_ptr) {
		bf->buf = r_buf_new_with_pointers (bytes, sz, true);
	} else {
		bf->buf = r_buf_new_with_bytes (bytes, sz);
	}
	return bf->buf;
}