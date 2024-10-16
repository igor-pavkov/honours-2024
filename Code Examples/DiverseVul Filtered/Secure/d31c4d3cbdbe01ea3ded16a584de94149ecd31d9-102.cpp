R_API RBinObject *r_bin_get_object(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	if (o) {
		o->referenced++;
	}
	return o;
}