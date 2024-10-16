R_API bool r_bin_file_delete(RBin *bin, ut32 bin_id) {
	r_return_val_if_fail (bin, false);

	RListIter *iter;
	RBinFile *bf, *cur = r_bin_cur (bin);

	r_list_foreach (bin->binfiles, iter, bf) {
		if (bf && bf->id == bin_id) {
			if (cur && cur->id == bin_id) {
				// avoiding UaF due to dead reference
				bin->cur = NULL;
			}
			r_list_delete (bin->binfiles, iter);
			return true;
		}
	}
	return false;
}