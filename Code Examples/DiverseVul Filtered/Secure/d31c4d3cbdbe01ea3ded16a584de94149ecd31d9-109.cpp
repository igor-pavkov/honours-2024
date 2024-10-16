static RBinFile *r_bin_file_xtr_load_bytes(RBin *bin, RBinXtrPlugin *xtr,
					    const char *filename,
					    const ut8 *bytes, ut64 sz,
					    ut64 file_sz, ut64 baseaddr,
					    ut64 loadaddr, int idx, int fd,
					    int rawstr) {
	if (!bin || !bytes) {
		return NULL;
	}
	RBinFile *bf = r_bin_file_find_by_name (bin, filename);
	if (!bf) {
		bf = r_bin_file_create_append (bin, filename, bytes, sz,
					       file_sz, rawstr, fd, xtr->name, false);
		if (!bf) {
			return NULL;
		}
		if (!bin->cur) {
			bin->cur = bf;
		}
	}
	if (bf->xtr_data) {
		r_list_free (bf->xtr_data);
	}
	if (xtr && bytes) {
		RList *xtr_data_list = xtr->extractall_from_bytes (bin, bytes, sz);
		RListIter *iter;
		RBinXtrData *xtr;
		//populate xtr_data with baddr and laddr that will be used later on
		//r_bin_file_object_new_from_xtr_data
		r_list_foreach (xtr_data_list, iter, xtr) {
			xtr->baddr = baseaddr? baseaddr : UT64_MAX;
			xtr->laddr = loadaddr? loadaddr : UT64_MAX;
		}
		bf->loadaddr = loadaddr;
		bf->xtr_data = xtr_data_list ? xtr_data_list : NULL;
	}
	return bf;
}