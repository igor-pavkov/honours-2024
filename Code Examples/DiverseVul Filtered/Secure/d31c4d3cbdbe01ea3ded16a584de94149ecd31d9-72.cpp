R_API RList *r_bin_get_strings(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->strings: NULL;
}