R_API int r_bin_has_dbg_syms(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? (R_BIN_DBG_SYMS & o->info->dbg_info): false;
}