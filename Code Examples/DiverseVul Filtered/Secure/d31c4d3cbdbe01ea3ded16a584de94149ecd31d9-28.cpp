R_API RList *r_bin_get_fields(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->fields: NULL;
}