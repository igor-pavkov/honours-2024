R_IPI RBinFile *r_bin_file_find_by_id(RBin *bin, ut32 bf_id) {
	RBinFile *bf;
	RListIter *iter;
	r_list_foreach (bin->binfiles, iter, bf) {
		if (bf->id == bf_id) {
			return bf;
		}
	}
	return NULL;
}