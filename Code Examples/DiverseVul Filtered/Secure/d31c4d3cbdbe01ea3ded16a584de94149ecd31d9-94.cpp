R_API RList *r_bin_get_entries(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->entries: NULL;
}