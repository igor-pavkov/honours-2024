R_IPI RBinFile *r_bin_file_xtr_load_buffer(RBin *bin, RBinXtrPlugin *xtr, const char *filename, RBuffer *buf, ut64 baseaddr, ut64 loadaddr, int idx, int fd, int rawstr) {
	r_return_val_if_fail (bin && xtr && buf, NULL);

	RBinFile *bf = r_bin_file_find_by_name (bin, filename);
	if (!bf) {
		bf = r_bin_file_new (bin, filename, r_buf_size (buf), rawstr, fd, xtr->name, bin->sdb, false);
		if (!bf) {
			return NULL;
		}
		r_list_append (bin->binfiles, bf);
		if (!bin->cur) {
			bin->cur = bf;
		}
	}
	r_list_free (bf->xtr_data);
	bf->xtr_data = NULL;
	if (xtr->extractall_from_buffer) {
		bf->xtr_data = xtr->extractall_from_buffer (bin, buf);
	} else if (xtr->extractall_from_bytes) {
		ut64 sz = 0;
		const ut8 *bytes = r_buf_data (buf, &sz);
		eprintf ("TODO: Implement extractall_from_buffer in '%s' xtr.bin plugin\n", xtr->name);
		bf->xtr_data = xtr->extractall_from_bytes (bin, bytes, sz);
	}
	if (bf->xtr_data) {
		RListIter *iter;
		RBinXtrData *x;
		//populate xtr_data with baddr and laddr that will be used later on
		//r_bin_file_object_new_from_xtr_data
		r_list_foreach (bf->xtr_data, iter, x) {
			x->baddr = baseaddr? baseaddr : UT64_MAX;
			x->laddr = loadaddr? loadaddr : UT64_MAX;
		}
	}
	bf->loadaddr = loadaddr;
	return bf;
}