R_API RBinInfo *r_bin_get_info(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->info: NULL;
}