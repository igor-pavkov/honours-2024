R_API RList * /*<RBinClass>*/ r_bin_get_classes(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->classes: NULL;
}