R_API RBinFile *r_bin_file_find_by_name(RBin *bin, const char *name) {
	RListIter *iter;
	RBinFile *bf = NULL;
	if (!bin || !name) {
		return NULL;
	}
	r_list_foreach (bin->binfiles, iter, bf) {
		if (bf && bf->file && !strcmp (bf->file, name)) {
			break;
		}
		bf = NULL;
	}
	return bf;
}