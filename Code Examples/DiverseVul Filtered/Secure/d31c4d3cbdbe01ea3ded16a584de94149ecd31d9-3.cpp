R_API int r_bin_is_stripped(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? (R_BIN_DBG_STRIPPED & o->info->dbg_info): 1;
}