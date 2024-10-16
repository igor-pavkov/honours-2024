R_API RList *r_bin_get_sections(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->sections: NULL;
}