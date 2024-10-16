R_API int r_bin_has_dbg_relocs(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? (R_BIN_DBG_RELOCS & o->info->dbg_info): false;
}