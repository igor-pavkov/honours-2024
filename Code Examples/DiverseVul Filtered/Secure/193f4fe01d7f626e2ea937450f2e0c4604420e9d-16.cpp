R_API ut64 r_bin_file_delete_all(RBin *bin) {
	if (bin) {
		ut64 counter = r_list_length (bin->binfiles);
		r_list_purge (bin->binfiles);
		bin->cur = NULL;
		return counter;
	}
	return 0;
}