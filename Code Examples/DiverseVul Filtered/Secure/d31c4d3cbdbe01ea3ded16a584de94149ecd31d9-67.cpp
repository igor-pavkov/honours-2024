R_API ut64 r_bin_get_boffset(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->boffset: UT64_MAX;
}