R_API int r_bin_has_dbg_linenums(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? (R_BIN_DBG_LINENUMS & o->info->dbg_info): false;
}