R_API int r_bin_is_big_endian(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return (o && o->info)? o->info->big_endian: -1;
}