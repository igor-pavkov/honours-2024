R_API int r_bin_is_static(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	if (o && r_list_length (o->libs) > 0)
		return R_BIN_DBG_STATIC & o->info->dbg_info;
	return true;
}